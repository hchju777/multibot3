"""pipeline.launch.py — D16 스모크 최소 배선.

다섯 실행 파일(mrta_node · mapf_node · sadg_t0_node · switch_selector_node ·
trajopt_node) + mrs_sim 셋(clock_node · state_integrator · roadmap_publisher)을
띄운다. `mrs_bringup`은 소스 0줄 규율을 지킨다 — 이 파일은 launch 구성일
뿐 알고리즘·서비스 로직을 담지 않는다(10_architecture.md:113, 357§7-2).

🔴 이 launch로 도는 실행은 어느 주장의 증거도 아니다(347§4-6) — result.json이
없고, 지표를 재지 않고, 반복 run·시드 스윕이 없다.

use_sim_time: true를 공통 dict로 전 노드에 일괄 주입한다 — clock_node
자신만 예외로 false를 받는다(자기가 낸 시간을 자기가 구독하는 순환을
피한다, clock_node.cpp 파일 주석 참조).

observation 인자는 **자리만 예약**한다(357 D28·Q5) — 이 라운드는 어떤 노드도
이 값을 소비하지 않는다. 관측 채널·observation_node는 만들지 않았다.
"""

import os

import yaml
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def _load_robots_roster(mrta_yaml_path):
    """F48-6(48차 코드 게이트) — 로봇 로스터의 단일 출처.

    `mrta.yaml`의 `mrta_node.ros__parameters.robots`를 읽어 `state_integrator`도
    같이 쓴다 — 두 곳에 같은 리스트를 손으로 맞춰 적지 않는다. `mrs_sim`은
    CN-7(`method.modules.*`) 트리 밖의 도구 패키지라 `mrta.yaml`을 ROS 파라미터
    파일로 직접 로드할 계약상의 근거는 없지만, 이 launch 파일 자신이 그 값을
    읽어 두 노드의 `parameters=[]`에 각각 넣어 주는 것은 CN-6/CN-7을 어기지
    않는다(알고리즘 파라미터가 여전히 `mrs_bringup/config/` 하나에만 산다).

    파일이 없으면(예: 시험이 임시 공유 디렉터리로 `get_package_share_directory`를
    monkeypatch한 경우) 빈 리스트로 되돌아간다 — launch 파싱 자체가 그것 때문에
    실패해서는 안 된다(F48-4 시험이 요구하는 것).
    """
    try:
        with open(mrta_yaml_path) as f:
            data = yaml.safe_load(f)
        return data["mrta_node"]["ros__parameters"]["robots"]
    except (FileNotFoundError, KeyError, TypeError):
        return []


def generate_launch_description():
    bringup_share = get_package_share_directory("mrs_bringup")

    default_roadmap_path = os.path.join(bringup_share, "config", "scenario", "roadmap.json")

    use_sim_time = LaunchConfiguration("use_sim_time")
    roadmap_path = LaunchConfiguration("roadmap_path")
    robot_specs_path = LaunchConfiguration("robot_specs_path")

    declare_use_sim_time = DeclareLaunchArgument(
        "use_sim_time",
        default_value="true",
        description="전 노드 공통 시뮬레이션 시간 사용 여부(CN, multibot3/CLAUDE.md 규칙).",
    )
    declare_roadmap_path = DeclareLaunchArgument(
        "roadmap_path",
        default_value=default_roadmap_path,
        description=(
            "roadmap_publisher가 읽을 mrs.roadmap JSON 파일 경로. "
            "기본값은 prototype/gen_roadmap.py 산출물을 mrs_bringup/config/scenario/로 "
            "그대로 복사한 것 — 값을 지어내지 않았다(347§4-4)."
        ),
    )
    declare_robot_specs_path = DeclareLaunchArgument(
        "robot_specs_path",
        default_value="",
        description=(
            "🔴 [값 부재] — robot_specs 소스 데이터가 이 라운드에 없다(prototype에 "
            "생성기·산출물 0건). 빈 문자열이면 roadmap_publisher가 /robot_specs를 "
            "발행하지 않고 WARN만 남긴다 — mapf_node/trajopt_node는 그 latched 토픽 "
            "없이는 로드맵만 받고 대기한다."
        ),
    )
    # 🔴 자리만 예약(357 D28) — 이 라운드 어떤 노드도 이 값을 읽지 않는다.
    declare_observation = DeclareLaunchArgument(
        "observation",
        default_value="off",
        description="[결정 부재 · 자리 예약] observation:=off|ranged. 관측 채널은 이 라운드 밖.",
    )

    common_params = {"use_sim_time": use_sim_time}

    mrta_yaml = os.path.join(bringup_share, "config", "mrta.yaml")
    mapf_yaml = os.path.join(bringup_share, "config", "mapf.yaml")
    sadg_yaml = os.path.join(bringup_share, "config", "sadg.yaml")
    trajopt_yaml = os.path.join(bringup_share, "config", "trajopt.yaml")

    # F48-6 해소 — 하드코딩 둘을 mrta.yaml 하나로 모았다(위 _load_robots_roster).
    robots = _load_robots_roster(mrta_yaml)

    nodes = [
        Node(
            package="mrs_mrta",
            executable="mrta_node",
            name="mrta_node",
            output="screen",
            parameters=[mrta_yaml, common_params],
        ),
        Node(
            package="mrs_mapf",
            executable="mapf_node",
            name="mapf_node",
            output="screen",
            parameters=[mapf_yaml, common_params],
        ),
        Node(
            package="mrs_sadg",
            executable="sadg_t0_node",
            name="sadg_t0_node",
            output="screen",
            parameters=[sadg_yaml, common_params],
        ),
        Node(
            package="mrs_sadg",
            executable="switch_selector_node",
            name="switch_selector_node",
            output="screen",
            parameters=[sadg_yaml, common_params],
        ),
        Node(
            package="mrs_trajopt",
            executable="trajopt_node",
            name="trajopt_node",
            output="screen",
            parameters=[trajopt_yaml, common_params],
        ),
        Node(
            package="mrs_sim",
            executable="clock_node",
            name="clock_node",
            output="screen",
            # clock_node 자신은 use_sim_time을 쓰지 않는다(자기 순환 회피).
            parameters=[{"use_sim_time": False}],
        ),
        Node(
            package="mrs_sim",
            executable="state_integrator",
            name="state_integrator",
            output="screen",
            parameters=[{"robots": robots}, common_params],
        ),
        Node(
            package="mrs_sim",
            executable="roadmap_publisher",
            name="roadmap_publisher",
            output="screen",
            parameters=[
                {"roadmap_path": roadmap_path, "robot_specs_path": robot_specs_path},
                common_params,
            ],
        ),
    ]

    return LaunchDescription(
        [
            declare_use_sim_time,
            declare_roadmap_path,
            declare_robot_specs_path,
            declare_observation,
            *nodes,
        ]
    )
