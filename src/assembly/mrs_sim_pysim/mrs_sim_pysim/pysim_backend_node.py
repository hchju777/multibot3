"""pysim_backend_node — pysim 백엔드 **프로세스** (architecture §2.5-a, sim-abstraction).

## 왜 별도 프로세스·Python 인가
최종 목표인 Isaac Sim 이 Python 호스팅 Kit 앱이다. pysim 을 C++ 인프로세스로 만들면 isaac 부착
시 프로세스 경계·비동기 스텝 의미론이 **그때 처음** 등장해 코어가 흔들린다(architecture C2).
pysim 을 처음부터 isaac 과 **같은 형태**(별도 프로세스 + 같은 `ISimBackend` 계약)로 두는 것이
진화 비용을 낮춘다. 런타임 임계 경로가 아니다(이산 스텝, 물리 최소).

## [0a] 에서 이 노드의 위치 — 정직하게
[0a] tracer bullet 은 **인프로세스 `FakeSimBackend`** 로 통과한다(오케스트레이터 지시).
이 노드가 지금 고정하는 것은 **프로세스의 형태**이지 [0a] 의 통과 경로가 아니다. C++ 쪽
프로세스 어댑터(`ISimBackend` 를 이 인터페이스 위에 구현하는 클래스)는 아직 없으며,
`sim_bridge` 는 `sim_backend:=pysim` 으로 기동하면 **fake 로 조용히 대체하지 않고 기동을
거부**한다. 즉 이 노드는 지금 단독으로 떠서 자기 계약면을 노출할 뿐이다.

## ⚠ 전송 바인딩 — 계약 레지스트리 **미등록** (contract-keeper 판단 필요)
공개 제어면 `/sim/{step,inject,query_capabilities}` 와 `/robot_{i}/{robot_state,cmd_vel}` 는
**`sim_bridge` 의 것**이다(계약 §3 정본표). 백엔드 프로세스가 같은 이름을 쓰면 서비스가 중복
등록되고 `RobotState` 의 "시뮬이 유일 발행자" 규약이 두 발행자로 깨진다. 그래서 이 노드는
**별도 네임스페이스 `/sim_backend`** 를 쓴다:

| 이름 | 타입 | 방향 | 대응 `ISimBackend` |
|---|---|---|---|
| `/sim_backend/step` | `mrs_interfaces/srv/SimStep` | bridge -> backend | `step()` |
| `/sim_backend/inject` | `mrs_interfaces/srv/SimInject` | bridge -> backend | `inject()` |
| `/sim_backend/query_capabilities` | `mrs_interfaces/srv/SimQueryCapabilities` | bridge -> backend | `capabilities()` |
| `/sim_backend/robot_{i}/observation` | `mrs_interfaces/msg/RobotState` | backend -> bridge | `sense()` |
| `/sim_backend/robot_{i}/actuation` | `geometry_msgs/msg/Twist` | bridge -> backend | `actuate()` |
| `/sim_backend/metrics_tap` | `mrs_interfaces/msg/SimMetricSample` | backend -> bridge | `poll_metrics()` |

`sense`/`actuate` 를 토픽으로 둔 것은 **계약 `.srv` 를 추가하지 않기 위해서다**(이번 작업은
`.msg`/`.srv` 수정 금지). 이 6개 이름은 아직 계약 레지스트리에 없으므로 **잠긴 계약이 아니며**,
등록 여부는 contract-keeper 가 판단한다.

## 시계
이 노드는 `/clock` 을 **발행하지 않는다** — 시간의 소유자는 `sim_bridge` 하나뿐이다
(architecture §1.7-B). 또 ROS 타이머를 하나도 쓰지 않는다. 세계는 오직 `/sim_backend/step`
호출로만 전진하므로 `use_sim_time` 값과 무관하게 부팅 데드락이 발생하지 않는다.

## 규율
- 콜백에 알고리즘 없음 — 운동학은 `kinematics.py`(순수 로직)에 있다.
- 모든 콜백은 try/except 로 감싼다. 실패는 로깅 + 안전 응답(현상유지)으로 흡수한다.
- 시각 변환은 `time_guard.seconds_to_time` 만 거친다(C++ `seconds_to_time` 의 미러).
"""
from typing import Callable, List, Optional

import rclpy
import rclpy.logging
from geometry_msgs.msg import Twist
from mrs_interfaces.msg import (
    ObservationUncertainty,
    Pose2D,
    RobotState,
    SimCapabilities,
    SimMetricSample,
    ViewScope,
)
from mrs_interfaces.srv import SimInject, SimQueryCapabilities, SimStep
from rclpy.callback_groups import MutuallyExclusiveCallbackGroup
from rclpy.node import Node
from rclpy.qos import DurabilityPolicy, QoSProfile, ReliabilityPolicy

from mrs_sim_pysim.kinematics import KinematicWorld, build_world
from mrs_sim_pysim.time_guard import seconds_to_time

#: 노드 id "해당 없음" 센티넬 (계약 §0 전역 규약).
NODE_ID_NONE: int = 4294967295

#: 로봇 id "해당 없음"/전역 지표 센티넬 (계약 §0 전역 규약).
ROBOT_ID_NONE: int = 65535

#: 이 백엔드의 이름. `SimCapabilities.backend_name` 허용 3종 중 하나여야 한다(계약 L-19).
BACKEND_NAME: str = 'pysim'

#: 로그 스로틀 간격 [s]. 스텝마다 도는 경로가 로그를 덮지 않게 한다.
LOG_THROTTLE_S: float = 2.0


class PysimBackendNode(Node):
    """pysim 이산 운동학 백엔드를 호스팅하는 프로세스 노드.

    `ISimBackend`(C++, `mrs/i_sim_backend.hpp`)의 5개 연산을 `/sim_backend/*` 경계 너머로
    노출한다. 상태·적분은 전부 `KinematicWorld`(순수 로직)가 갖고 있으며 이 클래스는
    얇은 ROS I/O 어댑터다.
    """

    def __init__(self) -> None:
        """파라미터를 조회해 세계를 구성하고 `/sim_backend/*` 경계면을 배선한다.

        구성에 실패하면 세계를 만들지 않고 `world` 를 `None` 으로 둔다 — 그 상태에서 들어온
        요청은 전부 실패 코드로 응답한다(추측한 구성으로 돌지 않는다).
        """
        super().__init__('pysim_backend')

        # [0a] 실측 대상 placeholder: sim_step_s 0.05 는 문헌·확정 근거가 없다.
        #   R-A2 상 Δt_h 는 이 값의 정수배여야 하며 그 검사는 mrs_bringup 이 수행한다.
        self.declare_parameter('sim_step_s', 0.05)
        self.declare_parameter('seed', 0)
        self.declare_parameter('robot_radius_m', 0.3)
        # 초기 배치는 **시나리오 구성값**이지 측정 대상이 아니다. 기본값은 [0a] 2대 마주보기.
        self.declare_parameter('initial_x_m', [0.0, 4.0])
        self.declare_parameter('initial_y_m', [0.0, 0.0])
        self.declare_parameter('initial_theta_rad', [0.0, 3.141592653589793])
        # 정식 경로는 MapRegistry GetUniformView 가 발급한 (roadmap_version, view_id) 다.
        #   두 기본값은 map_registry 의 roadmap_version·uniform_view_id 기본값과 같아야 한다.
        #   0 을 쓰지 않는 이유: roadmap_version 0 은 계약이 런타임 금지, view_id 0 은 물리 뷰
        #   예약값이라 균일 뷰가 0 이면 스코프 대조가 물리↔균일 혼동을 못 잡는다.
        self.declare_parameter('view_roadmap_version', 1)
        self.declare_parameter('view_uniform_view_id', 1)

        self.step_dt_s: float = float(self.get_parameter('sim_step_s').value)
        self.seed: int = int(self.get_parameter('seed').value)
        self.robot_radius_m: float = float(self.get_parameter('robot_radius_m').value)
        initial_x = [float(v) for v in self.get_parameter('initial_x_m').value]
        initial_y = [float(v) for v in self.get_parameter('initial_y_m').value]
        initial_theta = [float(v) for v in self.get_parameter('initial_theta_rad').value]
        self.roadmap_version: int = int(self.get_parameter('view_roadmap_version').value)
        self.view_id: int = int(self.get_parameter('view_uniform_view_id').value)

        self.world: Optional[KinematicWorld] = build_world(
            step_dt_s=self.step_dt_s,
            robot_radius_m=self.robot_radius_m,
            initial_x_m=initial_x,
            initial_y_m=initial_y,
            initial_theta_rad=initial_theta,
        )
        if self.world is None:
            self.get_logger().fatal(
                'pysim 세계 구성 실패 — sim_step_s(%f) 또는 초기 배치 배열 길이'
                '(x=%d, y=%d, theta=%d)를 확인하십시오. 값을 추측해서 채우지 않습니다.'
                % (self.step_dt_s, len(initial_x), len(initial_y), len(initial_theta))
            )

        self.discarded_actuation_count: int = 0
        self.time_guard_reject_count: int = 0
        self.next_injection_id: int = 0

        # 백엔드 상태 접근을 한 줄로 세운다 — 스텝과 구동이 끼어들면 결정론이 깨진다.
        self.backend_group = MutuallyExclusiveCallbackGroup()

        stream_qos = QoSProfile(
            depth=1,
            reliability=ReliabilityPolicy.BEST_EFFORT,
            durability=DurabilityPolicy.VOLATILE,
        )
        metrics_qos = QoSProfile(
            depth=50,
            reliability=ReliabilityPolicy.RELIABLE,
            durability=DurabilityPolicy.VOLATILE,
        )

        robot_count = len(initial_x)
        self.observation_pubs: List = []
        self.actuation_subs: List = []
        for robot_id in range(robot_count):
            self.observation_pubs.append(
                self.create_publisher(
                    RobotState, '/sim_backend/robot_%d/observation' % robot_id, stream_qos
                )
            )
            self.actuation_subs.append(
                self.create_subscription(
                    Twist,
                    '/sim_backend/robot_%d/actuation' % robot_id,
                    self._make_actuation_callback(robot_id),
                    stream_qos,
                    callback_group=self.backend_group,
                )
            )

        self.metrics_pub = self.create_publisher(
            SimMetricSample, '/sim_backend/metrics_tap', metrics_qos
        )

        self.step_srv = self.create_service(
            SimStep, '/sim_backend/step', self.on_step, callback_group=self.backend_group
        )
        self.inject_srv = self.create_service(
            SimInject, '/sim_backend/inject', self.on_inject, callback_group=self.backend_group
        )
        self.capabilities_srv = self.create_service(
            SimQueryCapabilities,
            '/sim_backend/query_capabilities',
            self.on_query_capabilities,
            callback_group=self.backend_group,
        )

        self.get_logger().info(
            'pysim_backend 기동 — robots=%d, sim_step_s=%f, seed=%d, ready=%s '
            '([0a] 는 인프로세스 FakeSimBackend 로 통과한다 — 이 프로세스는 형태 고정용이다)'
            % (robot_count, self.step_dt_s, self.seed, self.world is not None)
        )

    # ── 경계면 ────────────────────────────────────────────────────────────────
    def _make_actuation_callback(self, robot_id: int) -> Callable[[Twist], None]:
        """로봇 하나의 구동 지령 구독 콜백을 만든다 (클로저로 robot_id 를 고정).

        Args:
            robot_id: 이 콜백이 담당할 로봇 id. 자료형 `int`.

        Returns:
            `Callable[[geometry_msgs.msg.Twist], None]` — rclpy 구독에 넘길 콜백.
        """

        def callback(msg: Twist) -> None:
            self.on_actuation(robot_id, msg)

        return callback

    def on_actuation(self, robot_id: int, msg: Twist) -> None:
        """구동 지령을 세계에 반영한다 (`ISimBackend::actuate` 대응).

        Args:
            robot_id: 대상 로봇 id. 자료형 `int`.
            msg: 몸체 선속도(`linear.x`)·각속도(`angular.z`) 지령.
                자료형 `geometry_msgs.msg.Twist`.

        Returns:
            `None`
        """
        try:
            if self.world is None:
                return
            if not self.world.set_command(robot_id, msg.linear.x, msg.angular.z):
                self.discarded_actuation_count += 1
                self.get_logger().warning(
                    'robot_%d 구동 지령 폐기 (미지의 로봇 id 또는 비유한 값, 누적 %d) — '
                    '현상유지합니다.' % (robot_id, self.discarded_actuation_count),
                    throttle_duration_sec=LOG_THROTTLE_S,
                )
        except Exception as error:  # noqa: BLE001 - 노드가 죽지 않게 전면 흡수한다
            self.get_logger().error(
                'on_actuation 예외 (지령 무시): %s' % error,
                throttle_duration_sec=LOG_THROTTLE_S,
            )

    def on_step(self, request: SimStep.Request, response: SimStep.Response) -> SimStep.Response:
        """시뮬 시간을 명시적으로 전진시키고 관측·지표를 발행한다 (`ISimBackend::step` 대응).

        Args:
            request: `steps`(전진할 스텝 수, 0 은 1 로 취급)를 담은 요청.
                자료형 `mrs_interfaces.srv.SimStep.Request`.
            response: 채울 응답. 자료형 `mrs_interfaces.srv.SimStep.Response`.

        Returns:
            `mrs_interfaces.srv.SimStep.Response` — `result`(RESULT_OK / RESULT_BACKEND_ERROR),
            `sim_time`(전진 후 시뮬 시각), `step_count`(누적 스텝 수), `message`(실패 사유).
        """
        try:
            if self.world is None:
                response.result = SimStep.Response.RESULT_BACKEND_ERROR
                response.message = 'world not configured'
                return response

            sim_time_s = self.world.step(int(request.steps))
            stamp = seconds_to_time(sim_time_s)
            if stamp is None:
                self.time_guard_reject_count += 1
                response.result = SimStep.Response.RESULT_BACKEND_ERROR
                response.step_count = self.world.step_count
                response.message = 'TIME_CONVERSION_GUARD'
                return response

            response.result = SimStep.Response.RESULT_OK
            response.sim_time.sec = stamp[0]
            response.sim_time.nanosec = stamp[1]
            response.step_count = self.world.step_count
            response.message = ''

            self.publish_observations(sim_time_s)
            self.publish_metrics(sim_time_s)
            return response
        except Exception as error:  # noqa: BLE001
            self.get_logger().error('on_step 예외 (현상유지): %s' % error)
            response.result = SimStep.Response.RESULT_BACKEND_ERROR
            response.message = 'exception in pysim backend'
            return response

    def on_inject(
        self, request: SimInject.Request, response: SimInject.Response
    ) -> SimInject.Response:
        """결함을 주입한다 (`ISimBackend::inject` 대응).

        지원하는 결함은 `FAULT_ROBOT_STALL`·`FAULT_ROBOT_FAULT` 뿐이다. 통신 결함은
        `comms_emul` 소관이고 통로 개폐는 지도를 아는 계층의 일이라 여기서 흉내내지 않는다
        (`FakeSimBackend` 와 같은 규약).

        Args:
            request: 결함 종류·대상·강도·지속시간.
                자료형 `mrs_interfaces.srv.SimInject.Request`.
            response: 채울 응답. 자료형 `mrs_interfaces.srv.SimInject.Response`.

        Returns:
            `mrs_interfaces.srv.SimInject.Response` — `result`(ACCEPTED / UNSUPPORTED /
            BAD_TARGET), `injection_id`(접수 시 1 부터 증가), `message`.
        """
        try:
            if self.world is None:
                response.result = SimInject.Response.RESULT_UNSUPPORTED
                response.injection_id = 0
                response.message = 'world not configured'
                return response

            kind = int(request.fault_kind)
            supported = (
                SimInject.Request.FAULT_ROBOT_STALL,
                SimInject.Request.FAULT_ROBOT_FAULT,
            )
            if kind not in supported:
                # 미지원(종류)과 대상 부적합을 **구별해서** 응답한다(계약 §0.2 뭉갬 금지).
                response.result = SimInject.Response.RESULT_UNSUPPORTED
                response.injection_id = 0
                response.message = 'unsupported fault_kind for pysim (kinematic-only backend)'
                return response

            robot_id = int(request.target_robot_id)
            duration_s = float(request.duration_s)
            if duration_s < 0.0:
                response.result = SimInject.Response.RESULT_BAD_TARGET
                response.injection_id = 0
                response.message = 'duration_s must be >= 0 (0 = until released)'
                return response

            if kind == SimInject.Request.FAULT_ROBOT_STALL:
                accepted = self.world.apply_stall(robot_id, duration_s)
            else:
                accepted = self.world.apply_fault(robot_id)

            if not accepted:
                response.result = SimInject.Response.RESULT_BAD_TARGET
                response.injection_id = 0
                response.message = 'unknown target_robot_id'
                return response

            self.next_injection_id += 1
            response.result = SimInject.Response.RESULT_ACCEPTED
            response.injection_id = self.next_injection_id
            response.message = ''
            return response
        except Exception as error:  # noqa: BLE001
            self.get_logger().error('on_inject 예외 (안전 응답): %s' % error)
            response.result = SimInject.Response.RESULT_BAD_TARGET
            response.injection_id = 0
            response.message = 'exception in pysim backend'
            return response

    def on_query_capabilities(
        self,
        request: SimQueryCapabilities.Request,
        response: SimQueryCapabilities.Response,
    ) -> SimQueryCapabilities.Response:
        """백엔드 능력을 신고한다 (`ISimBackend::capabilities` 대응, Q-8).

        Args:
            request: 빈 요청. 자료형 `mrs_interfaces.srv.SimQueryCapabilities.Request`.
            response: 채울 응답. 자료형 `mrs_interfaces.srv.SimQueryCapabilities.Response`.

        Returns:
            `mrs_interfaces.srv.SimQueryCapabilities.Response` — `capabilities` 에 물리 충실도
            `FIDELITY_KINEMATIC`, 결정론 `True`, 접촉·부분관측 미지원이 실린다. 이 값이
            sim-runner 의 `OPEN(pending-isaac)` 자동 태깅 근거이므로 부풀리지 않는다.
        """
        del request
        try:
            capabilities = SimCapabilities()
            capabilities.backend_name = BACKEND_NAME
            capabilities.deterministic_with_seed = True
            capabilities.seed = self.seed
            capabilities.physics_fidelity = SimCapabilities.FIDELITY_KINEMATIC
            capabilities.supports_partial_observation = False
            capabilities.supports_contact = False
            capabilities.supports_fault_injection = True
            # 이산 스텝 백엔드라 공칭 배속은 호출 측 페이싱이 정한다. 1.0 은 "실시간 페이싱"이라는
            # 정의값이지 측정치가 아니다 — 실제 배속은 sim_bridge 가 realtime_factor 로 **측정**한다.
            capabilities.nominal_realtime_factor = 1.0
            capabilities.actuate_to_state_latency_s = 0.0
            response.capabilities = capabilities
            return response
        except Exception as error:  # noqa: BLE001
            self.get_logger().error('on_query_capabilities 예외: %s' % error)
            return response

    # ── 발행 ──────────────────────────────────────────────────────────────────
    def publish_observations(self, sim_time_s: float) -> None:
        """현재 세계 상태를 로봇별 관측 토픽으로 발행한다 (`ISimBackend::sense` 대응).

        노드 부착(`occupied_node`/`next_node`)은 **하지 않는다**. 이 프로세스는 MapRegistry 를
        모르므로 계약이 정한 정상 경로대로 센티넬을 싣는다 — 추측해서 붙이지 않는다.

        Args:
            sim_time_s: 이 관측이 유효한 시뮬 시각 [s]. 자료형 `float`.

        Returns:
            `None`
        """
        if self.world is None:
            return
        stamp = seconds_to_time(sim_time_s)
        if stamp is None:
            self.time_guard_reject_count += 1
            self.get_logger().warning(
                'observation 시각 가드 위반 — 발행하지 않습니다 (누적 %d).'
                % self.time_guard_reject_count,
                throttle_duration_sec=LOG_THROTTLE_S,
            )
            return

        for robot_id in self.world.robot_ids():
            if robot_id >= len(self.observation_pubs):
                continue
            state = self.world.robots[robot_id]

            msg = RobotState()
            msg.header.stamp.sec = stamp[0]
            msg.header.stamp.nanosec = stamp[1]
            msg.header.frame_id = 'map'
            msg.robot_id = robot_id

            pose = Pose2D()
            pose.x_m = state.x_m
            pose.y_m = state.y_m
            pose.theta_rad = state.theta_rad
            msg.pose = pose

            msg.v_mps = state.v_mps
            msg.omega_rps = state.omega_rps

            scope = ViewScope()
            scope.view_kind = ViewScope.VIEW_KIND_UNIFORM
            scope.roadmap_version = self.roadmap_version
            scope.view_id = self.view_id
            msg.view_scope = scope

            msg.occupied_node = NODE_ID_NONE
            msg.next_node = NODE_ID_NONE
            msg.edge_progress = 0.0

            uncertainty = ObservationUncertainty()
            uncertainty.pos_std_m = 0.0
            uncertainty.theta_std_rad = 0.0
            uncertainty.partial_observation = False
            uncertainty.observation_stamp.sec = stamp[0]
            uncertainty.observation_stamp.nanosec = stamp[1]
            msg.observation = uncertainty

            self.observation_pubs[robot_id].publish(msg)

    def publish_metrics(self, sim_time_s: float) -> None:
        """백엔드 고유 지표를 metrics tap 으로 발행한다 (`ISimBackend::poll_metrics` 대응).

        물리 충실도가 `KINEMATIC` 이므로 **모든 표본은 `TAG_OPEN_PENDING_ISAAC`** 이다 —
        여기서 나온 값을 연구 검증으로 승격하는 것을 구조적으로 막는다(architecture §5.3).

        Args:
            sim_time_s: 표본 시각 [s]. 자료형 `float`.

        Returns:
            `None`
        """
        if self.world is None:
            return
        stamp = seconds_to_time(sim_time_s)
        if stamp is None:
            self.time_guard_reject_count += 1
            return

        for key, value in self.world.collect_metrics():
            sample = SimMetricSample()
            sample.header.stamp.sec = stamp[0]
            sample.header.stamp.nanosec = stamp[1]
            sample.key = key
            sample.value = value
            sample.robot_id = ROBOT_ID_NONE
            sample.fidelity_tag = SimMetricSample.TAG_OPEN_PENDING_ISAAC
            self.metrics_pub.publish(sample)


def main(args: Optional[List[str]] = None) -> None:
    """pysim_backend_node 실행 파일 진입점.

    Args:
        args: `rclpy.init` 에 전달할 CLI 인자. 자료형 `Optional[List[str]]`.
            None 이면 `sys.argv` 를 쓴다.

    Returns:
        `None`
    """
    rclpy.init(args=args)
    node = None
    try:
        node = PysimBackendNode()
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    except Exception as error:  # noqa: BLE001 - 기동 실패를 조용히 삼키지 않는다
        rclpy.logging.get_logger('pysim_backend').fatal('기동 실패: %s' % error)
    finally:
        if node is not None:
            node.destroy_node()
        if rclpy.ok():
            rclpy.shutdown()


if __name__ == '__main__':
    main()
