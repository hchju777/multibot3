"""_common.launch.py — mrs_bringup 공용 조립 로직 (Composition Root).

pysim.launch.py / isaac.launch.py 가 이 파일을 sim_backend 인자만 바꿔 재사용한다 —
"백엔드 선택은 config 한 줄"(architecture §5.1, scaffolding 통과 기준)의 실현.

노드 실행 순서: bringup_startup_check(R-A2) 통과 -> 서버 노드군 -> plan_tick_node -> l4_traj_node xN.
l4_traj_node 는 robot_count 를 런타임에 해석해야 하므로 OpaqueFunction 으로 동적 생성한다
(LaunchConfiguration 은 빌드 시점이 아니라 실행 시점에 값이 정해지기 때문).

이 파일은 배선(WHO talks to WHOM)만 담당한다 — 알고리즘은 없다(ros2-conventions 규율).
"""
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, OpaqueFunction
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def _launch_setup(context, *args, **kwargs):
    robot_count = int(LaunchConfiguration("robot_count").perform(context))
    sim_backend = LaunchConfiguration("sim_backend").perform(context)
    replan_period_s = float(LaunchConfiguration("replan_period_s").perform(context))
    sim_step_s = float(LaunchConfiguration("sim_step_s").perform(context))

    actions = [
        Node(
            package="mrs_bringup",
            executable="startup_check_exe",
            name="bringup_startup_check",
            output="screen",
            parameters=[{"replan_period_s": replan_period_s, "sim_step_s": sim_step_s}],
        ),
        Node(
            package="mrs_ros_map_registry",
            executable="map_registry_node_exe",
            name="map_registry",
            output="screen",
        ),
        Node(
            package="mrs_ros_ta",
            executable="ta_service_node_exe",
            name="ta_service",
            output="screen",
        ),
        Node(
            package="mrs_ros_pp",
            executable="pp_service_node_exe",
            name="pp_service",
            output="screen",
        ),
        Node(
            package="mrs_ros_sadg",
            executable="sadg_service_node_exe",
            name="sadg_service",
            output="screen",
            parameters=[{"robot_count": robot_count}],
        ),
        Node(
            package="mrs_ros_ladder",
            executable="ladder_orchestrator_node_exe",
            name="ladder_orchestrator",
            output="screen",
            parameters=[{"robot_count": robot_count}],
        ),
        Node(
            package="mrs_ros_plan_tick",
            executable="plan_tick_node_exe",
            name="plan_tick_node",
            output="screen",
            parameters=[{"replan_period_s": replan_period_s}],
        ),
        Node(
            package="mrs_ros_sim_bridge",
            executable="sim_bridge_node_exe",
            name="sim_bridge",
            output="screen",
            parameters=[{"robot_count": robot_count, "sim_backend": sim_backend}],
        ),
    ]

    for robot_id in range(robot_count):
        neighbor_ids = [i for i in range(robot_count) if i != robot_id]
        actions.append(
            Node(
                package="mrs_ros_l4",
                executable="l4_traj_node_exe",
                name="l4_traj_node",
                namespace=f"robot_{robot_id}",
                output="screen",
                parameters=[{"robot_id": robot_id, "neighbor_robot_ids": neighbor_ids}],
            )
        )

    return actions


def generate_launch_description():
    return LaunchDescription(
        [
            DeclareLaunchArgument("robot_count", default_value="2"),
            DeclareLaunchArgument("sim_backend", default_value="pysim"),
            DeclareLaunchArgument("replan_period_s", default_value="0.1"),
            # [0a] 실측 대상 placeholder — 수치 창작 금지 규율, tracer bullet [0a] 실측 후 확정.
            DeclareLaunchArgument("sim_step_s", default_value="0.05"),
            OpaqueFunction(function=_launch_setup),
        ]
    )
