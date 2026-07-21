"""isaac.launch.py — isaac 백엔드로 전 노드 기동 (isaac 충실도 티어, 진화 단계 미부착).

사용: ros2 launch mrs_bringup isaac.launch.py [robot_count:=2]
⚠ mrs_sim_isaac 은 현재 NotImplementedError 스텁이다 — 이 launch 파일은 배선 자리만 확정한다.
"""
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, DeclareLaunchArgument
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
import os


def generate_launch_description():
    common = os.path.join(os.path.dirname(__file__), "_common.launch.py")
    return LaunchDescription(
        [
            DeclareLaunchArgument("robot_count", default_value="2"),
            IncludeLaunchDescription(
                PythonLaunchDescriptionSource(common),
                launch_arguments={
                    "robot_count": LaunchConfiguration("robot_count"),
                    "sim_backend": "isaac",
                }.items(),
            ),
        ]
    )
