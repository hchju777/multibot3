cmake_minimum_required(VERSION 3.28)
project(mrs_ros_ladder)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()
add_compile_options(-Wall -Wextra -Wpedantic -Werror)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(mrs_interfaces REQUIRED)
find_package(mrs_msg_convert REQUIRED)
find_package(mrs_ladder REQUIRED)
find_package(mrs_core_types REQUIRED)
# RungEventRecord(= to_msg(RungEvent) 의 입력 도메인 타입)이 mrs_metrics 에 있다.
find_package(mrs_metrics REQUIRED)
find_package(builtin_interfaces REQUIRED)
# /map_registry/roadmap_version (std_msgs/UInt64, transient_local) — 기대 뷰 스코프 학습 경로.
find_package(std_msgs REQUIRED)

file(GLOB_RECURSE MRS_ROS_LADDER_SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_executable(ladder_orchestrator_node_exe ${MRS_ROS_LADDER_SOURCES})
target_include_directories(ladder_orchestrator_node_exe PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include>")
target_compile_features(ladder_orchestrator_node_exe PUBLIC cxx_std_17)
ament_target_dependencies(ladder_orchestrator_node_exe PUBLIC
  rclcpp
  mrs_interfaces
  mrs_msg_convert
  mrs_ladder
  mrs_core_types
  mrs_metrics
  builtin_interfaces
  std_msgs)

install(TARGETS ladder_orchestrator_node_exe DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include)

if(BUILD_TESTING)
  # gtest 골격은 Phase 5(test-author)가 내용과 함께 생성한다 (design-decision v5 build_policy).
endif()

ament_package()
