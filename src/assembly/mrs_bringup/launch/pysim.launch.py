"""pysim.launch.py — pysim 백엔드로 전 노드 기동 (unit/pysim 충실도 티어).

사용: ros2 launch mrs_bringup pysim.launch.py [robot_count:=2]
"""
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, ThisLaunchFileDir
from launch.actions import DeclareLaunchArgument
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
                    "sim_backend": "pysim",
                }.items(),
            ),
        ]
    )
