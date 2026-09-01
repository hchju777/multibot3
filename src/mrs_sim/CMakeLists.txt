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

add_executable(clock_node src/node/clock_node.cpp)
ament_target_dependencies(clock_node rclcpp rosgraph_msgs)
target_link_libraries(clock_node mrs_core::mrs_core_pure)

add_executable(state_integrator src/node/state_integrator.cpp)
ament_target_dependencies(state_integrator rclcpp geometry_msgs nav_msgs)

add_executable(roadmap_publisher src/node/roadmap_publisher.cpp)
ament_target_dependencies(roadmap_publisher rclcpp mrs_msgs)
target_link_libraries(roadmap_publisher
  mrs_core::mrs_core_pure mrs_core::mrs_core_msgs nlohmann_json::nlohmann_json)

install(TARGETS clock_node state_integrator roadmap_publisher
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
endif()

ament_package()
