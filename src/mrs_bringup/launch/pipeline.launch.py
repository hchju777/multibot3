"""pipeline.launch.py — D16 스모크 최소 배선.

다섯 실행 파일(mrta_node · mapf_node · sadg_t0_node · switch_selector_node ·
trajopt_node) + mrs_sim 여섯(clock_node · state_integrator · roadmap_publisher ·
task_release_publisher · viz_markers_node, 367_pipeline_inputs.md 웨이브 1-A +
373_viz_markers.md 48차 웨이브 2-B · observation_node, 375_observation_
publisher.md 48차 웨이브 4-A, observation:=ranged일 때만·기본 off) + mrs_viz
하나(dashboard_node, 374_mrs_viz_dashboard.md 48차 웨이브 3) + 선택적 rviz2를
띄운다. `mrs_bringup`은 소스 0줄 규율을 지킨다 — 이 파일은 launch 구성일 뿐
알고리즘·서비스 로직을 담지 않는다(10_architecture.md:113, 357§7-2).

🔴 374_mrs_viz_dashboard.md — `dashboard_node`는 관찰 전용 웹 대시보드다(GET만
서빙, 조작 UI 0개). `dashboard` 런치 인자(기본 `true`)로 켜고 끈다 — `rviz`와
달리 디스플레이가 필요 없어(HTTP 포트를 열 뿐) 헤드리스 환경에서도 기본으로
띄운다. 포트가 이미 쓰이는 CI 환경 등에서는 `dashboard:=false`로 끌 수 있다.

🔴 373_viz_markers.md — `viz_markers_node`는 이미 있는 경계 토픽(/roadmap,
/{robot}/odom, /execution_constraints)을 구독해 rviz2 마커를 낼 뿐, 네
알고리즘 모듈 코드를 한 줄도 건드리지 않는다(U48-8). `rviz` 런치 인자
(기본 `false`)로 rviz2 프로세스 자체를 같이 띄울지 고른다 — 기본을 꺼둔
이유는 디스플레이가 없는 헤드리스 CI/스모크 환경에서 이 launch가 깨지지
않게 하기 위함이다(`observation` 인자와 같은 "자리는 있되 기본 off" 관례).

🔴 이 launch로 도는 실행은 어느 주장의 증거도 아니다(347§4-6) — result.json이
없고, 지표를 재지 않고, 반복 run·시드 스윕이 없다. 🔴 robot_specs_path·
task_release_path의 기본값은 **가정 데이터**다(U48-5) — 실제 로봇에서 온 값이
아니므로 이 값으로 돈 run은 SC-* 증거로 계상될 수 없다
(config/scenario/README.md).

use_sim_time: true를 공통 dict로 전 노드에 일괄 주입한다 — clock_node
자신만 예외로 false를 받는다(자기가 낸 시간을 자기가 구독하는 순환을
피한다, clock_node.cpp 파일 주석 참조).

🔴 375_observation_publisher.md(48차 웨이브 4-A) — `observation` 인자가 이제
**실제 소비자를 갖는다**: `observation:=ranged`면 `mrs_sim/observation_node`가
로봇마다 `/{robot}/scan`(sensor_msgs/LaserScan)을 낸다(357§Q1이 확정한 이음매,
BEST_EFFORT/VOLATILE/1). **기본값은 여전히 `off`**다 — D16 스모크(357 D28)의
기존 범위·노드 수·소요 시간을 이 라운드가 조용히 재정의하지 않기 위한 보수적
선택이고, 이 노드를 켜고 끄는 결정은 `system-architect`의 몫으로 열어 둔다.
`observation:=off`면 `observation_node`는 아예 뜨지 않는다(IfCondition — rviz·
dashboard와 같은 조건부 패턴). 차단 이벤트 출처는 시나리오 파일뿐이다(U46-3의
두 출처 중 런타임 토글은 다음 라운드, 소비 측이 선 뒤).

🔴 367_pipeline_inputs_p2.md — `state_integrator`에 `initial_vertices`를 `robots`와
같은 출처(`mrta.yaml`)에서 읽어 넘긴다. `trajopt_node`(첫 /odom 대기)와
`state_integrator`(첫 /cmd_vel 대기)의 순환 대기를 끊는 값이다.
"""

import os

import yaml
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration, PythonExpression
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


def _load_initial_vertices_roster(mrta_yaml_path):
    """367_pipeline_inputs_p2.md — 순환 대기 처리의 값 출처.

    `mrta.yaml`의 `mrta_node.ros__parameters.initial_vertices`(367_pipeline_
    inputs.md §3이 이미 세운 것 — prototype/out/assignment.json의
    `assignments[].start`를 그대로 옮긴 값)를 `state_integrator`도 같이 쓴다.
    같은 값을 두 곳에 손으로 맞춰 적지 않는다(`_load_robots_roster`와 같은
    이유). 파일이 없거나 키가 없으면 빈 리스트 — `state_integrator`는 그 경우
    초기 `/odom` 시드를 내지 않고 WARN만 남긴다(state_integrator.cpp).
    """
    try:
        with open(mrta_yaml_path) as f:
            data = yaml.safe_load(f)
        return data["mrta_node"]["ros__parameters"]["initial_vertices"]
    except (FileNotFoundError, KeyError, TypeError):
        return []


def generate_launch_description():
    bringup_share = get_package_share_directory("mrs_bringup")

    default_roadmap_path = os.path.join(bringup_share, "config", "scenario", "roadmap.json")
    default_robot_specs_path = os.path.join(bringup_share, "config", "scenario", "robot_specs.json")
    default_task_release_path = os.path.join(
        bringup_share, "config", "scenario", "task_release.json"
    )
    # 375_observation_publisher.md — observation:=ranged일 때만 observation_node가
    # 읽는다(가정 데이터, config/scenario/README.md "blocked_edges.json" 절).
    default_blocked_edges_path = os.path.join(
        bringup_share, "config", "scenario", "blocked_edges.json"
    )

    use_sim_time = LaunchConfiguration("use_sim_time")
    roadmap_path = LaunchConfiguration("roadmap_path")
    robot_specs_path = LaunchConfiguration("robot_specs_path")
    task_release_path = LaunchConfiguration("task_release_path")
    blocked_edges_path = LaunchConfiguration("blocked_edges_path")

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
        default_value=default_robot_specs_path,
        description=(
            "🔴 [가정 데이터, U48-5] — 실제 로봇 등록 스펙이 이 라운드에 없어 "
            "config/scenario/robot_specs.json(README.md '가정 데이터')을 기본값으로 "
            "쓴다. 빈 문자열로 오버라이드하면 roadmap_publisher가 /robot_specs를 "
            "발행하지 않고 WARN만 남긴다(옛 동작 보존)."
        ),
    )
    declare_task_release_path = DeclareLaunchArgument(
        "task_release_path",
        default_value=default_task_release_path,
        description=(
            "🔴 [가정 데이터의 파생, U48-5 정신 승계] — task_release_publisher가 읽을 "
            "mrs.task_release 봉투 배열 JSON. config/scenario/task_release.json은 "
            "prototype/out/assignment.json(같은 instance_id)에서 역산한 것이며 새 값을 "
            "지어내지 않았다(README.md 'task_release.json'). 빈 문자열로 오버라이드하면 "
            "task_release_publisher가 /task_release를 발행하지 않고 WARN만 남긴다."
        ),
    )
    # 375_observation_publisher.md — 실제 소비자를 갖는다(observation_node).
    # 기본 off(D16 스모크 범위 보수적 유지) — ranged로 켜면 로봇당 /{robot}/scan을 낸다.
    declare_observation = DeclareLaunchArgument(
        "observation",
        default_value="off",
        description="observation:=off|ranged. ranged면 mrs_sim/observation_node가 뜬다.",
    )
    declare_blocked_edges_path = DeclareLaunchArgument(
        "blocked_edges_path",
        default_value=default_blocked_edges_path,
        description=(
            "🔴 [가정 데이터] observation_node가 읽을 시각별 차단 이벤트 시나리오 "
            "(config/scenario/blocked_edges.json, mrs.* 계약 스키마가 아니다 — README.md "
            "'blocked_edges.json' 절). 빈 문자열로 오버라이드하면 어떤 간선도 막히지 않는다."
        ),
    )
    # 373_viz_markers.md — rviz2 프로세스를 같이 띄울지. 기본 false(헤드리스
    # 환경에서 이 launch의 파싱/스모크가 디스플레이 유무에 흔들리지 않게).
    declare_rviz = DeclareLaunchArgument(
        "rviz",
        default_value="false",
        description="true면 rviz2를 config/viz/pipeline.rviz로 같이 띄운다.",
    )
    # 374_mrs_viz_dashboard.md — 관찰 전용 웹 대시보드. 기본 true(디스플레이
    # 불필요, HTTP 포트만 연다) — 포트 충돌이 우려되는 환경에서만 끈다.
    declare_dashboard = DeclareLaunchArgument(
        "dashboard",
        default_value="true",
        description="true면 mrs_viz/dashboard_node(관찰 전용 웹 대시보드)를 같이 띄운다.",
    )

    common_params = {"use_sim_time": use_sim_time}

    mrta_yaml = os.path.join(bringup_share, "config", "mrta.yaml")
    mapf_yaml = os.path.join(bringup_share, "config", "mapf.yaml")
    sadg_yaml = os.path.join(bringup_share, "config", "sadg.yaml")
    trajopt_yaml = os.path.join(bringup_share, "config", "trajopt.yaml")
    viz_yaml = os.path.join(bringup_share, "config", "viz.yaml")
    viz_dashboard_yaml = os.path.join(bringup_share, "config", "viz_dashboard.yaml")
    observation_yaml = os.path.join(bringup_share, "config", "observation.yaml")
    rviz_config_path = os.path.join(bringup_share, "config", "viz", "pipeline.rviz")
    # 374_mrs_viz_dashboard.md — 웹 자산은 mrs_viz가 설치한 share/mrs_viz/web/에
    # 산다(mrs_bringup은 소스 0줄이라 자산도 안 갖는다). 설치 경로 의존값이라
    # config/viz_dashboard.yaml에는 없다(그 파일 머리말 표가 이유를 적는다).
    dashboard_web_root = os.path.join(
        get_package_share_directory("mrs_viz"), "web"
    )

    # F48-6 해소 — 하드코딩 둘을 mrta.yaml 하나로 모았다(위 _load_robots_roster).
    robots = _load_robots_roster(mrta_yaml)
    # 367_pipeline_inputs_p2.md — state_integrator의 초기 /odom 시드 값 출처.
    initial_vertices = _load_initial_vertices_roster(mrta_yaml)

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
            parameters=[
                {"robots": robots, "initial_vertices": initial_vertices},
                common_params,
            ],
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
        Node(
            package="mrs_sim",
            executable="task_release_publisher",
            name="task_release_publisher",
            output="screen",
            parameters=[
                {"task_release_path": task_release_path},
                common_params,
            ],
        ),
        # 373_viz_markers.md (U48-8) — /roadmap · /{robot}/odom ·
        # /execution_constraints를 구독해 /viz/markers를 낸다. `robots`는
        # state_integrator와 같은 출처(F48-6 정신) — 손으로 다시 안 적는다.
        Node(
            package="mrs_sim",
            executable="viz_markers_node",
            name="viz_markers_node",
            output="screen",
            parameters=[
                viz_yaml,
                {"robots": robots},
                common_params,
            ],
        ),
        # 375_observation_publisher.md (48차 웨이브 4-A) — 로봇당 /{robot}/scan.
        # observation:=ranged일 때만 실제로 뜬다(IfCondition) — 파싱 시점엔 항상 존재.
        # `robots`는 state_integrator·viz_markers_node와 같은 출처(F48-6 정신).
        Node(
            package="mrs_sim",
            executable="observation_node",
            name="observation_node",
            output="screen",
            parameters=[
                observation_yaml,
                {"robots": robots, "blocked_edges_path": blocked_edges_path},
                common_params,
            ],
            condition=IfCondition(
                PythonExpression(["'", LaunchConfiguration("observation"), "' == 'ranged'"])
            ),
        ),
        # 374_mrs_viz_dashboard.md (48차 웨이브 3) — 관찰 전용 웹 대시보드.
        # dashboard:=true(기본)일 때만 실제로 뜬다 — 파싱 시점엔 항상 존재.
        # `robots`는 state_integrator·viz_markers_node와 같은 출처(F48-6 정신).
        # `web_root`는 설치 경로 의존값이라 config yaml이 아니라 이 launch가
        # 계산해 넘긴다(위 dashboard_web_root, viz_dashboard.yaml 머리말 표).
        Node(
            package="mrs_viz",
            executable="dashboard_node",
            name="dashboard_node",
            output="screen",
            parameters=[
                viz_dashboard_yaml,
                {"robots": robots, "web_root": dashboard_web_root},
                common_params,
            ],
            condition=IfCondition(LaunchConfiguration("dashboard")),
        ),
        # rviz:=true일 때만 실제로 뜬다(IfCondition) — 파싱 시점엔 항상 존재.
        Node(
            package="rviz2",
            executable="rviz2",
            name="rviz2",
            output="screen",
            arguments=["-d", rviz_config_path],
            parameters=[common_params],
            condition=IfCondition(LaunchConfiguration("rviz")),
        ),
    ]

    return LaunchDescription(
        [
            declare_use_sim_time,
            declare_roadmap_path,
            declare_robot_specs_path,
            declare_task_release_path,
            declare_observation,
            declare_blocked_edges_path,
            declare_rviz,
            declare_dashboard,
            *nodes,
        ]
    )
