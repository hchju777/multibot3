"""isaac_backend_node — Isaac Sim 어댑터 자리 (architecture §2.5, sim-abstraction R-01).

**지금은 스텁이다.** 이 파일의 목적은 알고리즘이 아니라 **계약 확정**이다 — 아래 메서드
시그니처는 C++ `ISimBackend` 포트(mrs_sim_abstraction/include/mrs/i_sim_backend.hpp)와
1:1 대응하도록 지금 고정해 둔다. 진화 단계(post-MVP, Isaac Sim 5.1 + ROS2 Jazzy 브리지,
impl-feasibility 가 실측한 드라이버 리스크 해소 후)에서 이 스텁을 채운다.

pysim(mrs_sim_pysim)과 이 스텁이 **같은 메서드 집합**을 갖는 것 자체가 sim-abstraction 규율의
실현이다 — 코어·시나리오·지표 파서는 어느 쪽이 채워지든 재작성되지 않는다.

⚠ 물리 충실도(SimCapabilities.physics_fidelity = FIDELITY_FULL_PHYSICS)가 필요한 지표
(QP feasibility rate·livelock rate·물리 재현성)는 이 어댑터가 채워지기 전까지
`OPEN(pending-isaac)` 로 자동 태깅된다(architecture §5.3, R-01 조건 3) — 사람이 이 판정을
손으로 뒤집는 것은 금지다.
"""
from typing import Dict, List, Tuple

import rclpy
from rclpy.node import Node


class IsaacBackendNode(Node):
    """Isaac Sim 백엔드 어댑터 — 전 메서드 NotImplementedError (진화 단계 대상).

    메서드 시그니처는 `mrs_sim_pysim.pysim_backend_node.PysimBackendNode` 및 C++
    `ISimBackend` 와 의도적으로 동일하다 — 백엔드 선택은 config 한 줄(`sim_backend` 파라미터)
    이어야 하며, 이는 두 어댑터가 같은 인터페이스를 실현할 때만 성립한다.
    """

    def __init__(self) -> None:
        """노드를 초기화한다. Isaac Sim Kit 앱 연결은 아직 수행하지 않는다."""
        super().__init__('isaac_backend')
        self.get_logger().warn(
            'isaac_backend started as STUB — Isaac Sim 미연결. 진화 단계(post-MVP) 대상.')

    def reset(self, seed: int) -> bool:
        """시뮬을 초기 상태로 되돌린다 (ISimBackend::reset 대응).

        Args:
            seed: 난수 시드. isaac 은 물리 비결정성이 있어 완전 재현은 보장되지 않는다
                (SimCapabilities.deterministic_with_seed = false 예상).

        Returns:
            성공 여부.
        """
        raise NotImplementedError('not implemented: IsaacBackendNode.reset — 진화 단계(post-MVP) 대상')

    def step(self, steps: int) -> float:
        """시뮬 시간을 명시적으로 전진시킨다 (ISimBackend::step 대응).

        Args:
            steps: 전진할 스텝 수.

        Returns:
            전진 후 시뮬 시각 [s].
        """
        raise NotImplementedError('not implemented: IsaacBackendNode.step — 진화 단계(post-MVP) 대상')

    def sense(self) -> List[Dict[str, float]]:
        """현재 로봇 상태를 관측한다 (ISimBackend::sense 대응, 실물리 관측 불확실성 포함 예정).

        Returns:
            로봇별 관측 딕셔너리 목록.
        """
        raise NotImplementedError('not implemented: IsaacBackendNode.sense — 진화 단계(post-MVP) 대상')

    def actuate(self, commands: List[Tuple[int, float, float]]) -> bool:
        """구동 지령을 적용한다 (ISimBackend::actuate 대응).

        Args:
            commands: (robot_id, v_mps, omega_rps) 튜플 목록.

        Returns:
            성공 여부.
        """
        raise NotImplementedError('not implemented: IsaacBackendNode.actuate — 진화 단계(post-MVP) 대상')

    def inject(self, fault_kind: int, target_id: int, magnitude: float, duration_s: float) -> int:
        """결함을 주입한다 (ISimBackend::inject 대응).

        Args:
            fault_kind: FaultKind enum 값.
            target_id: 대상 로봇/엣지 id.
            magnitude: 결함 강도.
            duration_s: 지속 시간 [s].

        Returns:
            해제·감사용 injection_id.
        """
        raise NotImplementedError('not implemented: IsaacBackendNode.inject — 진화 단계(post-MVP) 대상')

    def poll_metrics(self) -> List[Dict[str, float]]:
        """metrics tap 표본을 회수한다 (ISimBackend::poll_metrics 대응, FULL_PHYSICS 태깅 대상).

        Returns:
            표본 딕셔너리 목록.
        """
        raise NotImplementedError(
            'not implemented: IsaacBackendNode.poll_metrics — 진화 단계(post-MVP) 대상')


def main(args: None = None) -> None:
    """isaac_backend_node 실행 파일 진입점.

    Args:
        args: rclpy.init 에 전달할 CLI 인자. None 이면 sys.argv 사용.
    """
    rclpy.init(args=args)
    node = IsaacBackendNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
