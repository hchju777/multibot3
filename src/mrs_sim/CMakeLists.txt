cmake_minimum_required(VERSION 3.16)
project(mrs_sim CXX)

# 48차 통합(P4) — 최소 시뮬레이터. 이 패키지는 CN-1의 6레이어 규율 대상이
# 아니다(도구 패키지) — include/mrs_sim/ 공개 헤더를 두지 않아 세 노드 전부
# .cpp 단독 골격이다(네 알고리즘 모듈의 node/ 골격과 같은 형태, 46차 판정
# "공개 헤더 없는 골격이라 합법"의 선례를 따른다).

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Wpedantic)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(mrs_msgs REQUIRED)
find_package(mrs_core REQUIRED)
find_package(rosgraph_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(std_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)

add_executable(clock_node src/node/clock_node.cpp)
ament_target_dependencies(clock_node rclcpp rosgraph_msgs)
target_link_libraries(clock_node mrs_core::mrs_core_pure)

# 367_pipeline_inputs_p2.md — 순환 대기 처리: /roadmap 구독(정점 좌표) +
# mrs_core::ContractViolation(초기 pose 시드 설정 검사) 신설.
add_executable(state_integrator src/node/state_integrator.cpp)
ament_target_dependencies(state_integrator rclcpp geometry_msgs nav_msgs mrs_msgs)
target_link_libraries(state_integrator mrs_core::mrs_core_pure)

add_executable(roadmap_publisher src/node/roadmap_publisher.cpp)
ament_target_dependencies(roadmap_publisher rclcpp mrs_msgs)
target_link_libraries(roadmap_publisher
  mrs_core::mrs_core_pure mrs_core::mrs_core_msgs nlohmann_json::nlohmann_json)

# 367_pipeline_inputs.md 웨이브 1-A ② — /task_release 생산자(D29 선례를 따른
# 통합 담당의 배치 판정, system-architect 확인 요망 — 367_pipeline_inputs.md §2).
add_executable(task_release_publisher src/node/task_release_publisher.cpp)
ament_target_dependencies(task_release_publisher rclcpp mrs_msgs)
target_link_libraries(task_release_publisher
  mrs_core::mrs_core_pure nlohmann_json::nlohmann_json)

# 373_viz_markers.md (48차 웨이브 2-B, U48-8) — /roadmap · /{robot}/odom ·
# /execution_constraints를 구독해 /viz/markers(visualization_msgs)를 낸다.
# 모듈 코드 0줄 — 이미 있는 경계 토픽의 재표현일 뿐이다.
add_executable(viz_markers_node src/node/viz_markers_node.cpp)
ament_target_dependencies(viz_markers_node
  rclcpp mrs_msgs nav_msgs std_msgs visualization_msgs)
target_link_libraries(viz_markers_node mrs_core::mrs_core_pure)

# 375_observation_publisher.md (48차 웨이브 4-A) — /{robot}/scan(sensor_msgs/
# LaserScan) 발행자. /roadmap(간선 기하) + /{robot}/odom(로봇 pose) + 시나리오
# 파일(시각별 차단 이벤트)만 읽는다. 모듈 코드(mrs_trajopt) 0줄 접촉.
add_executable(observation_node src/node/observation_node.cpp)
ament_target_dependencies(observation_node rclcpp mrs_msgs nav_msgs sensor_msgs)
target_link_libraries(observation_node
  mrs_core::mrs_core_pure nlohmann_json::nlohmann_json)

install(TARGETS clock_node state_integrator roadmap_publisher task_release_publisher
  viz_markers_node observation_node
  DESTINATION lib/${PROJECT_NAME})

# --- tests (F48-4/F48-5, 48차 코드 게이트) ---
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()  # <test_depend>ament_lint_auto</test_depend> 소비.

  enable_testing()
  add_executable(test_roadmap_parse_roundtrip test/test_roadmap_parse_roundtrip.cpp)
  target_link_libraries(test_roadmap_parse_roundtrip
    mrs_core::mrs_core_pure mrs_core::mrs_core_msgs nlohmann_json::nlohmann_json)
  ament_target_dependencies(test_roadmap_parse_roundtrip mrs_msgs)
  add_test(NAME test_roadmap_parse_roundtrip COMMAND test_roadmap_parse_roundtrip)

  add_executable(test_blocked_edges_parse test/test_blocked_edges_parse.cpp)
  target_link_libraries(test_blocked_edges_parse
    mrs_core::mrs_core_pure nlohmann_json::nlohmann_json)
  add_test(NAME test_blocked_edges_parse COMMAND test_blocked_edges_parse)
endif()

ament_package()
