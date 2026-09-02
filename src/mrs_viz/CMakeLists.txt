cmake_minimum_required(VERSION 3.16)
project(mrs_viz CXX)

# 48차 웨이브 3(374_mrs_viz_dashboard.md, U47-2·U48-4) — 관찰 전용 웹 대시보드.
# 도구 패키지 — include/mrs_viz/ 공개 헤더를 두지 않는다(mrs_sim 선례를 따른다,
# check_layer_layout.py가 "미검사"로 신고할 뿐 위반이 아니다). 알고리즘 교체
# 지점이 없어 CN-1 필수 넷(core/adapter/node/service)이 안 맞는다 — 판정과
# 사유는 374_mrs_viz_dashboard.md §「패키지 레이아웃」.

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Wpedantic)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(mrs_msgs REQUIRED)
find_package(mrs_core REQUIRED)
find_package(action_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Threads REQUIRED)

add_executable(dashboard_node
  src/node/dashboard_node.cpp
  src/node/dashboard_state.cpp)
ament_target_dependencies(dashboard_node
  rclcpp mrs_msgs action_msgs geometry_msgs nav_msgs)
target_link_libraries(dashboard_node
  mrs_core::mrs_core_pure mrs_core::mrs_core_msgs
  nlohmann_json::nlohmann_json Threads::Threads)

install(TARGETS dashboard_node DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY web DESTINATION share/${PROJECT_NAME})

# --- tests (374_mrs_viz_dashboard.md — ROS 없이 도는 순수 로직 시험) ---
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()

  enable_testing()

  add_executable(test_http_request_parse test/test_http_request_parse.cpp)
  add_test(NAME test_http_request_parse COMMAND test_http_request_parse)

  add_executable(test_dashboard_snapshot_json
    test/test_dashboard_snapshot_json.cpp
    src/node/dashboard_state.cpp)
  target_link_libraries(test_dashboard_snapshot_json nlohmann_json::nlohmann_json)
  add_test(NAME test_dashboard_snapshot_json COMMAND test_dashboard_snapshot_json)
endif()

ament_package()
