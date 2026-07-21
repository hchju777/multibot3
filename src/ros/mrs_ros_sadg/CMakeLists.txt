cmake_minimum_required(VERSION 3.28)
project(mrs_ros_sadg)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()
add_compile_options(-Wall -Wextra -Wpedantic -Werror)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
# 전용 콜백그룹 실행자 스레드(계약 Q-3 — 릴리스 경로 격리)가 std::thread 를 쓴다.
find_package(Threads REQUIRED)
find_package(mrs_interfaces REQUIRED)
find_package(mrs_msg_convert REQUIRED)
find_package(mrs_depgraph REQUIRED)
find_package(mrs_reorder_btpg REQUIRED)
find_package(mrs_reorder_milp REQUIRED)
find_package(mrs_judge REQUIRED)

file(GLOB_RECURSE MRS_ROS_SADG_SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_executable(sadg_service_node_exe ${MRS_ROS_SADG_SOURCES})
target_include_directories(sadg_service_node_exe PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include>")
target_compile_features(sadg_service_node_exe PUBLIC cxx_std_17)
ament_target_dependencies(sadg_service_node_exe PUBLIC rclcpp mrs_interfaces mrs_msg_convert mrs_depgraph mrs_reorder_btpg mrs_reorder_milp mrs_judge)
target_link_libraries(sadg_service_node_exe PUBLIC Threads::Threads)

install(TARGETS sadg_service_node_exe DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include)

if(BUILD_TESTING)
  # gtest 골격은 Phase 5(test-author)가 내용과 함께 생성한다 (design-decision v5 build_policy).
endif()

ament_package()
