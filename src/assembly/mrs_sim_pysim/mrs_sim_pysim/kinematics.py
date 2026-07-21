"""kinematics — pysim 백엔드의 순수 운동학 (ROS 무의존, 결정론).

`ISimBackend` 의 pysim 구현이 실제로 하는 일은 이 모듈에 전부 있다. ROS 배선
(`pysim_backend_node.py`)과 분리한 이유는 두 가지다.

1. **순수 로직 분리**(ros2-conventions) — 콜백 안에 알고리즘을 두지 않는다. 여기 있는 함수는
   ROS 없이 pytest 로 검증할 수 있다.
2. **`FakeSimBackend` 와 동등성 확인 가능** — [0a] 는 인프로세스 `FakeSimBackend` 로 통과하고
   pysim 은 프로세스 **형태**를 고정하는 것이 목적이므로, 두 구현의 운동학이 같아야 백엔드
   교체가 결과를 바꾸지 않는다는 주장이 성립한다. 아래 적분식·충돌 판정은
   `domain/mrs_sim_abstraction/src/fake_sim_backend.cpp::advance_one_step` 을 그대로 옮긴 것이다.

## 충실도 정직성 (architecture §5.3)
기하·이산 운동학만 있다. 접촉력·동역학·물리 비결정성이 없으므로 **QP feasibility·livelock·
실제 diff-drive 물리는 이 백엔드로 판정할 수 없다.** 이 사실은 `SimCapabilities.physics_fidelity`
= `KINEMATIC` 로 신고되며, 여기서 나오는 모든 지표는 `pending_isaac = True` 로 나간다.
"""
import math
from dataclasses import dataclass, field
from typing import Dict, List, Optional, Tuple


def normalize_angle(theta_rad: float) -> float:
    """각도를 `[-pi, pi]` 구간으로 접는다.

    Args:
        theta_rad: 방위각 [rad]. 자료형 `float`. 임의의 실수.

    Returns:
        `float` — `[-pi, pi]` 안으로 접힌 방위각 [rad]. 계약 §0 전역 규약의 각도 구간이다.
    """
    wrapped = math.fmod(theta_rad + math.pi, 2.0 * math.pi)
    if wrapped < 0.0:
        wrapped += 2.0 * math.pi
    return wrapped - math.pi


@dataclass
class RobotKinematicState:
    """로봇 1대의 내부 운동 상태 (몸체점 q 기준).

    Attributes:
        robot_id: 로봇 id. 자료형 `int`.
        x_m: 위치 x [m], map 프레임.
        y_m: 위치 y [m], map 프레임.
        theta_rad: 방위각 [rad], `[-pi, pi]`.
        v_mps: 현재 적용 중인 선속도 [m/s].
        omega_rps: 현재 적용 중인 각속도 [rad/s].
        stalled: `FAULT_ROBOT_STALL` 로 일시 정지 중인가.
        stall_until_s: 정지 해제 시각 [s]. 0.0 이면 "해제 전까지 무기한".
        faulted: `FAULT_ROBOT_FAULT` 로 영구 고장인가.
    """

    robot_id: int
    x_m: float = 0.0
    y_m: float = 0.0
    theta_rad: float = 0.0
    v_mps: float = 0.0
    omega_rps: float = 0.0
    stalled: bool = False
    stall_until_s: float = 0.0
    faulted: bool = False


@dataclass
class KinematicWorld:
    """pysim 세계 전체 — 결정론적 상태와 구간 지표.

    Attributes:
        step_dt_s: 시뮬 스텝 dt [s]. 계약 R-A2 상 Δt_h 는 이 값의 정수배여야 한다.
        robot_radius_m: 로봇 반지름 [m]. 기하 충돌 판정에만 쓴다.
        sim_time_s: 현재 시뮬 시각 [s].
        step_count: `reset()` 이후 누적 스텝 수(결정론 재현 키).
        robots: 로봇 id → 상태. 순회는 항상 id 오름차순으로 한다(결정론적 순서).
        collision_count: 누적 기하 충돌 횟수.
        min_separation_m: 마지막 회수 이후 관측된 최소 이격 거리 [m].
        separation_seen: 이번 구간에 이격 거리를 한 번이라도 관측했는가.
    """

    step_dt_s: float = 0.05
    robot_radius_m: float = 0.3
    sim_time_s: float = 0.0
    step_count: int = 0
    robots: Dict[int, RobotKinematicState] = field(default_factory=dict)
    collision_count: int = 0
    min_separation_m: float = 0.0
    separation_seen: bool = False

    def robot_ids(self) -> List[int]:
        """결정론적 순회 순서(로봇 id 오름차순)를 돌려준다.

        Returns:
            `List[int]` — 정렬된 로봇 id 목록.
        """
        return sorted(self.robots.keys())

    def set_command(self, robot_id: int, v_mps: float, omega_rps: float) -> bool:
        """구동 지령을 적용한다 (지연 없음 — `actuate_to_state_latency_s = 0` 대응).

        Args:
            robot_id: 대상 로봇 id. 자료형 `int`.
            v_mps: 선속도 지령 [m/s]. 자료형 `float`. 비유한 값은 거부된다.
            omega_rps: 각속도 지령 [rad/s]. 자료형 `float`. 비유한 값은 거부된다.

        Returns:
            `bool` — 적용했으면 True. 미지의 로봇 id 이거나 비유한 지령이면 False
            (호출자는 폐기 + 로그 + 카운터로 처리한다).
        """
        if robot_id not in self.robots:
            return False
        if not math.isfinite(v_mps) or not math.isfinite(omega_rps):
            return False
        state = self.robots[robot_id]
        state.v_mps = float(v_mps)
        state.omega_rps = float(omega_rps)
        return True

    def advance_one_step(self) -> None:
        """스텝 1회를 적분한다 (정지 만료 판정 → 유니사이클 적분 → 충돌·최소이격 갱신).

        정지 만료는 **구간 시작 시각**으로 판정한다 — 끝 시각으로 판정하면 정지 구간에 완전히
        포함된 스텝에서도 로봇이 움직여 한 스텝 일찍 재개된다(`FakeSimBackend` 와 동일 규약).

        Returns:
            `None`
        """
        step_start_s = self.sim_time_s
        self.sim_time_s += self.step_dt_s

        for robot_id in self.robot_ids():
            state = self.robots[robot_id]

            if state.faulted:
                state.v_mps = 0.0
                state.omega_rps = 0.0
                continue

            if state.stalled:
                # stall_until_s == 0.0 은 "해제 전까지 무기한"이다(FaultInjection.duration_s 규약).
                if state.stall_until_s > 0.0 and step_start_s >= state.stall_until_s:
                    state.stalled = False
                else:
                    continue

            dt = self.step_dt_s
            state.x_m += state.v_mps * math.cos(state.theta_rad) * dt
            state.y_m += state.v_mps * math.sin(state.theta_rad) * dt
            state.theta_rad = normalize_angle(state.theta_rad + state.omega_rps * dt)

        # 기하 충돌·최소 이격 갱신. 접촉력은 모형화하지 않으며 겹침을 되밀지도 않는다 —
        # 겹친 채로 두고 세는 것이 "기하 전용"의 정직한 거동이다.
        contact_distance = 2.0 * self.robot_radius_m
        ids = self.robot_ids()
        for i in range(len(ids)):
            for j in range(i + 1, len(ids)):
                a = self.robots[ids[i]]
                b = self.robots[ids[j]]
                separation = math.hypot(a.x_m - b.x_m, a.y_m - b.y_m)
                if (not self.separation_seen) or separation < self.min_separation_m:
                    self.min_separation_m = separation
                    self.separation_seen = True
                if separation < contact_distance:
                    self.collision_count += 1

        self.step_count += 1

    def step(self, steps: int) -> float:
        """시뮬 시간을 명시적으로 전진시킨다.

        Args:
            steps: 전진할 스텝 수. 자료형 `int`. 0 이하는 1 로 취급한다.

        Returns:
            `float` — 전진 후 시뮬 시각 [s].
        """
        effective = steps if steps > 0 else 1
        for _ in range(effective):
            self.advance_one_step()
        return self.sim_time_s

    def apply_stall(self, robot_id: int, duration_s: float) -> bool:
        """로봇을 일시 정지시킨다 (`FAULT_ROBOT_STALL`).

        Args:
            robot_id: 대상 로봇 id. 자료형 `int`.
            duration_s: 지속 시간 [s]. 자료형 `float`. 0.0 이면 해제 전까지 무기한.

        Returns:
            `bool` — 접수했으면 True, 미지의 로봇 id 이면 False.
        """
        if robot_id not in self.robots:
            return False
        state = self.robots[robot_id]
        state.stalled = True
        state.stall_until_s = (self.sim_time_s + duration_s) if duration_s > 0.0 else 0.0
        return True

    def apply_fault(self, robot_id: int) -> bool:
        """로봇을 영구 고장 상태로 만든다 (`FAULT_ROBOT_FAULT`).

        Args:
            robot_id: 대상 로봇 id. 자료형 `int`.

        Returns:
            `bool` — 접수했으면 True, 미지의 로봇 id 이면 False.
        """
        if robot_id not in self.robots:
            return False
        self.robots[robot_id].faulted = True
        return True

    def collect_metrics(self) -> List[Tuple[str, float]]:
        """마지막 회수 이후의 metrics tap 표본을 가져간다(회수 후 구간 지표는 리셋된다).

        Returns:
            `List[Tuple[str, float]]` — `(key, value)` 목록. 누적 지표 `collision_count` 는
            항상 포함되고, 구간 지표 `min_separation_m` 은 관측이 있었을 때만 포함된다.
            **모든 표본은 호출자가 `pending_isaac = True` 로 실어야 한다**(§5.3).
        """
        samples: List[Tuple[str, float]] = [("collision_count", float(self.collision_count))]
        if self.separation_seen:
            samples.append(("min_separation_m", self.min_separation_m))
        self.separation_seen = False
        self.min_separation_m = 0.0
        return samples


def build_world(
    step_dt_s: float,
    robot_radius_m: float,
    initial_x_m: List[float],
    initial_y_m: List[float],
    initial_theta_rad: List[float],
) -> Optional[KinematicWorld]:
    """초기 배치로부터 세계를 만든다.

    잘못된 구성으로 조용히 도는 것보다 기동을 막는 편이 낫다(`FakeSimBackend::configure` 와
    같은 취지) — 배열 길이가 어긋나거나 스텝 dt 가 0 이하이면 `None` 을 돌려준다.

    Args:
        step_dt_s: 시뮬 스텝 dt [s]. 자료형 `float`. 0 이하이면 거부.
        robot_radius_m: 로봇 반지름 [m]. 자료형 `float`. 음수이면 거부.
        initial_x_m: 로봇별 초기 x [m]. 자료형 `List[float]`. 길이가 로봇 수를 정한다.
        initial_y_m: 로봇별 초기 y [m]. 자료형 `List[float]`. `initial_x_m` 과 길이가 같아야 한다.
        initial_theta_rad: 로봇별 초기 방위각 [rad]. 자료형 `List[float]`. 같은 길이여야 한다.

    Returns:
        `Optional[KinematicWorld]` — 구성이 유효하면 세계, 아니면 `None`.
    """
    if step_dt_s <= 0.0 or robot_radius_m < 0.0:
        return None
    count = len(initial_x_m)
    if len(initial_y_m) != count or len(initial_theta_rad) != count or count == 0:
        return None

    world = KinematicWorld(step_dt_s=step_dt_s, robot_radius_m=robot_radius_m)
    for index in range(count):
        world.robots[index] = RobotKinematicState(
            robot_id=index,
            x_m=float(initial_x_m[index]),
            y_m=float(initial_y_m[index]),
            theta_rad=normalize_angle(float(initial_theta_rad[index])),
        )
    return world
