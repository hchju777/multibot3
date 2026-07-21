"""isaac.launch.py — isaac 충실도 티어 전 구간 기동 (진화 단계, **현재 미부착**).

사용: ros2 launch mrs_bringup isaac.launch.py [robot_count:=2]

⚠ 현재 이 launch 는 **뜨지 않는다.** `sim_backend:=isaac` 이면 `sim_bridge` 가 기동을 거부한다
(C++ isaac 어댑터 미구현, `mrs_sim_isaac` 은 스텁). 이것은 결함이 아니라 의도다 — 조용히 fake
로 대체하면 pysim 충실도 결과가 isaac 결과로 오독될 수 있고, 그것이 kill-gate 정직성 손상의
정확한 형태다(architecture §5.3, R-01 조건 3).

이 파일이 지금 하는 일은 **배선 자리 확정**이다: isaac 부착 시 바뀌는 것은 `sim_backend` 인자
한 줄뿐이며 노드 그래프·토픽·파라미터는 그대로여야 한다(architecture C2).

⚠ 미결 Q-14: isaac 티어에서 `/clock` 발행자가 `sim_bridge` 인지 Isaac ROS2 브리지인지 아직
확정되지 않았다. 둘 다 발행하면 시계가 둘이 되어 R-A1(틱 = 시계의 순수 함수)이 무너진다.
[0b] 실측 대상이며 추정하지 않는다.
"""
import os

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration

#: `_common.launch.py` 로 전달하는 인자. `sim_backend` 만 isaac 으로 고정된다.
_FORWARDED_ARGUMENTS = (
    ('robot_count', '2'),
    ('replan_period_s', '0.1'),
    ('sim_step_s', '0.05'),
    ('step_wall_period_s', '0.05'),
    ('initial_x_m', ''),
    ('initial_y_m', ''),
    ('initial_theta_rad', ''),
)


def generate_launch_description() -> LaunchDescription:
    """isaac 티어 launch 기술을 만든다.

    Returns:
        `LaunchDescription` — 인자 선언 + `_common.launch.py` 포함(`sim_backend='isaac'` 고정).
    """
    common = os.path.join(os.path.dirname(__file__), '_common.launch.py')
    declarations = [
        DeclareLaunchArgument(name, default_value=default)
        for name, default in _FORWARDED_ARGUMENTS
    ]
    forwarded = {name: LaunchConfiguration(name) for name, _ in _FORWARDED_ARGUMENTS}
    forwarded['sim_backend'] = 'isaac'

    return LaunchDescription(
        declarations
        + [
            IncludeLaunchDescription(
                PythonLaunchDescriptionSource(common),
                launch_arguments=forwarded.items(),
            ),
        ]
    )
