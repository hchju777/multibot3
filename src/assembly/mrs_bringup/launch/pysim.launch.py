"""pysim.launch.py — pysim **충실도 티어** 전 구간 기동 ([0a] tracer bullet 의 진입점).

사용:
    ros2 launch mrs_bringup pysim.launch.py
    ros2 launch mrs_bringup pysim.launch.py robot_count:=2 sim_step_s:=0.05
    ros2 launch mrs_bringup pysim.launch.py sim_backend:=pysim   # 프로세스 백엔드 시도(현재 거부)

## ⚠ 티어 이름과 백엔드 이름은 다르다
`pysim` 은 **충실도 티어**의 이름이고(architecture §6: unit / pysim / isaac), `sim_backend` 는
**백엔드 구현**의 이름이다. [0a] 는 pysim **티어**를 인프로세스 `FakeSimBackend` **백엔드**로
돌린다 — 오케스트레이터가 "[0a] 는 FakeSimBackend 인프로세스로 통과 가능"이라고 지시했고,
그 백엔드는 이미 구현·검증됐기 때문이다. 그래서 `sim_backend` 기본값이 `fake` 다.

`sim_backend:=pysim` 을 주면 `sim_bridge` 는 **기동을 거부**한다(C++ 프로세스 어댑터 미구현).
조용히 fake 로 대체하지 않는 이유는 `SimCapabilities.backend_name` 이 실제와 달라지는 순간
sim-runner 의 `OPEN(pending-isaac)` 자동 태깅이 근거를 잃기 때문이다(architecture §5.3).

## ⚠ [0a] 통과는 [0] 통과가 아니다 (오케스트레이터 R-01 조건 1~3)
여기서 나오는 배속·지연 수치는 **배관 실측**이다. pysim 은 Isaac 배속·물리 재현성 분산을
만들어낼 수 없으므로, 이 수치로 이후 실험의 N·반복 수를 확정하지 말 것(TBD-pending-[0b]).
"""
import os

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration

#: `_common.launch.py` 에서 그대로 전달하는 인자 목록. 여기 없는 인자는 공용 파일의 기본값을 쓴다.
_FORWARDED_ARGUMENTS = (
    ('robot_count', '2'),
    ('sim_backend', 'fake'),
    ('replan_period_s', '0.1'),
    ('sim_step_s', '0.05'),
    ('step_wall_period_s', '0.05'),
    ('initial_x_m', ''),
    ('initial_y_m', ''),
    ('initial_theta_rad', ''),
    ('launch_pysim_process', 'false'),
)


def generate_launch_description() -> LaunchDescription:
    """pysim 티어 launch 기술을 만든다.

    Returns:
        `LaunchDescription` — 인자 선언 + `_common.launch.py` 포함(백엔드 인자 전달).
    """
    common = os.path.join(os.path.dirname(__file__), '_common.launch.py')
    declarations = [
        DeclareLaunchArgument(name, default_value=default)
        for name, default in _FORWARDED_ARGUMENTS
    ]
    forwarded = {name: LaunchConfiguration(name) for name, _ in _FORWARDED_ARGUMENTS}

    return LaunchDescription(
        declarations
        + [
            IncludeLaunchDescription(
                PythonLaunchDescriptionSource(common),
                launch_arguments=forwarded.items(),
            ),
        ]
    )
