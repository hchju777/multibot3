"""pysim_backend_node — pysim 백엔드 프로세스 (architecture §2.5-a, sim-abstraction).

왜 별도 프로세스·Python 인가: 최종 목표인 Isaac Sim 이 Python 호스팅 Kit 앱이다. pysim 을
C++ 인프로세스로 만들면 isaac 부착 시 프로세스 경계·비동기 스텝 의미론이 **그때 처음**
등장해 코어가 흔들린다(architecture C2 위반). pysim 을 처음부터 isaac 과 **같은 형태**
(별도 프로세스 + 같은 ISimBackend 계약)로 두는 것이 진화 비용을 낮춘다.

경량·물리 최소·이산·결정론(시드 고정)이 이 백엔드의 규율이다(sim-abstraction SKILL).
C++ 쪽 `ISimBackend` 포트(mrs_sim_abstraction/include/mrs/i_sim_backend.hpp)와 개념적으로
1:1 대응하는 메서드 집합을 두되, 이 노드가 sim_bridge(C++, mrs_ros_sim_bridge)와 실제로
어떤 프로토콜로 통신할지(ROS2 서비스 프록시 / 별도 IPC)는 **Phase 5(coordination-builder)
미정 사항**이다 — 지금은 백엔드 프로세스의 자리와 계약 대응 시그니처만 고정한다.
"""
from typing import Dict, List, Tuple

import rclpy
from rclpy.node import Node


class PysimBackendNode(Node):
    """pysim 물리 최소 이산 시뮬 백엔드를 호스팅하는 노드.

    ISimBackend(C++, mrs/i_sim_backend.hpp)와 개념적으로 대응하는 메서드 집합을 제공한다.
    본문은 전부 NotImplementedError — 알고리즘(경량 운동학 적분)은 Phase 5 대상이다.
    """

    def __init__(self) -> None:
        """노드를 초기화한다. 파라미터 선언만 수행하며 물리 루프는 아직 시작하지 않는다."""
        super().__init__('pysim_backend')
        self.declare_parameter('seed', 0)
        self.declare_parameter('sim_step_s', 0.05)  # [0a] 실측 대상 placeholder
        self.get_logger().info('pysim_backend started (stub) — physics loop not yet implemented')

    def reset(self, seed: int) -> bool:
        """시뮬을 초기 상태로 되돌리고 시드를 고정한다 (ISimBackend::reset 대응).

        Args:
            seed: 난수 시드. 재현성 실험의 키.

        Returns:
            성공 여부.
        """
        raise NotImplementedError('not implemented: PysimBackendNode.reset — Phase 5 대상')

    def step(self, steps: int) -> float:
        """시뮬 시간을 명시적으로 전진시킨다 (ISimBackend::step 대응).

        Args:
            steps: 전진할 스텝 수.

        Returns:
            전진 후 시뮬 시각 [s].
        """
        raise NotImplementedError('not implemented: PysimBackendNode.step — Phase 5 대상')

    def sense(self) -> List[Dict[str, float]]:
        """현재 로봇 상태를 관측한다 (ISimBackend::sense 대응).

        Returns:
            로봇별 관측 딕셔너리 목록(위치·속도·불확실성 — pysim 은 0/이상화 값).
        """
        raise NotImplementedError('not implemented: PysimBackendNode.sense — Phase 5 대상')

    def actuate(self, commands: List[Tuple[int, float, float]]) -> bool:
        """구동 지령을 적용한다 (ISimBackend::actuate 대응).

        Args:
            commands: (robot_id, v_mps, omega_rps) 튜플 목록.

        Returns:
            성공 여부.
        """
        raise NotImplementedError('not implemented: PysimBackendNode.actuate — Phase 5 대상')

    def inject(self, fault_kind: int, target_id: int, magnitude: float, duration_s: float) -> int:
        """결함을 주입한다 (ISimBackend::inject 대응).

        Args:
            fault_kind: FaultKind enum 값 (mrs/i_sim_backend.hpp 와 값 일치).
            target_id: 대상 로봇/엣지 id.
            magnitude: 결함 강도.
            duration_s: 지속 시간 [s]. 0 = 해제 전까지.

        Returns:
            해제·감사용 injection_id.
        """
        raise NotImplementedError('not implemented: PysimBackendNode.inject — Phase 5 대상')

    def poll_metrics(self) -> List[Dict[str, float]]:
        """마지막 step 이후 누적된 metrics tap 표본을 회수한다 (ISimBackend::poll_metrics 대응).

        Returns:
            표본 딕셔너리 목록(key, value, robot_id, pending_isaac).
        """
        raise NotImplementedError('not implemented: PysimBackendNode.poll_metrics — Phase 5 대상')


def main(args: None = None) -> None:
    """pysim_backend_node 실행 파일 진입점.

    Args:
        args: rclpy.init 에 전달할 CLI 인자. None 이면 sys.argv 사용.
    """
    rclpy.init(args=args)
    node = PysimBackendNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
