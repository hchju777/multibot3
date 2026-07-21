cmake_minimum_required(VERSION 3.28)
project(mrs_ros_l4)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()
add_compile_options(-Wall -Wextra -Wpedantic -Werror)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(mrs_interfaces REQUIRED)
find_package(mrs_msg_convert REQUIRED)
find_package(mrs_local_traj REQUIRED)
find_package(mrs_core_types REQUIRED)
# RobotObservation(= from_msg(RobotState) 의 출력 도메인 타입)이 mrs/i_sim_backend.hpp 에 있다.
# mrs_msg_convert 가 전이로 노출하지만, 헤더에서 직접 include 하므로 명시 의존으로 둔다.
find_package(mrs_sim_abstraction REQUIRED)
find_package(builtin_interfaces REQUIRED)
find_package(geometry_msgs REQUIRED)
# /map_registry/roadmap_version (std_msgs/UInt64, transient_local) — 기대 뷰 스코프 학습 경로.
find_package(std_msgs REQUIRED)

file(GLOB_RECURSE MRS_ROS_L4_SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_executable(l4_traj_node_exe ${MRS_ROS_L4_SOURCES})
target_include_directories(l4_traj_node_exe PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include>")
target_compile_features(l4_traj_node_exe PUBLIC cxx_std_17)
ament_target_dependencies(l4_traj_node_exe PUBLIC
  rclcpp
  mrs_interfaces
  mrs_msg_convert
  mrs_local_traj
  mrs_core_types
  mrs_sim_abstraction
  builtin_interfaces
  geometry_msgs
  std_msgs)

install(TARGETS l4_traj_node_exe DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include)

if(BUILD_TESTING)
  # gtest 골격은 Phase 5(test-author)가 내용과 함께 생성한다 (design-decision v5 build_policy).
endif()

ament_package()
