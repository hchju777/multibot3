cmake_minimum_required(VERSION 3.16)
project(mrs_trajopt CXX)

# 🔴 ROS 2 런타임이 이 체크아웃에 없다(brief §1). 그래서 이 CMakeLists는 **순수 CMake**로
#    코어 라이브러리 + 시험 + 측정 실행기를 빌드한다. ament/rclcpp/rosidl 절은 통합 라운드에서
#    ROS 머신으로 옮길 때 켠다 — 아래 "ROS WRAPPER (DISABLED)" 블록 참조.

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Wpedantic)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# --- core (CN-2/CN-3): pure algorithm. No ROS, no rosidl, no JSON, no mrs_msgs. ---
add_library(mrs_trajopt_core STATIC
  src/core/velocity_profiler.cpp
  src/core/startup_checks.cpp
  src/core/control_tick_service.cpp)
target_include_directories(mrs_trajopt_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# --- plugins (CN-5): swap-target implementations. Link core only. ---
add_library(mrs_trajopt_plugins STATIC
  src/plugins/search_hybrid_astar.cpp
  src/plugins/search_fixed_path_gating.cpp
  src/plugins/subgoal_candidates.cpp
  src/plugins/peer_channel_impls.cpp)
target_link_libraries(mrs_trajopt_plugins PUBLIC mrs_trajopt_core)

# --- adapter (CN-8/CN-9): internal <-> boundary structs. No JSON. ---
add_library(mrs_trajopt_adapter STATIC
  src/adapter/trajectories_adapter.cpp
  src/adapter/robot_specs_adapter.cpp
  src/adapter/stop_declaration_adapter.cpp)
target_link_libraries(mrs_trajopt_adapter PUBLIC mrs_trajopt_core)

# --- io: JSON (de)serialization. Kept out of core/adapter (CN-2/CN-3). ---
add_library(mrs_trajopt_io STATIC
  src/io/json_io.cpp)
target_link_libraries(mrs_trajopt_io PUBLIC mrs_trajopt_adapter)

# --- measurement spike runner (output c) ---
add_executable(mrs_trajopt_spike spike/measure_spike.cpp)
target_link_libraries(mrs_trajopt_spike PRIVATE
  mrs_trajopt_core mrs_trajopt_plugins mrs_trajopt_adapter mrs_trajopt_io)

# --- tests (CTest; no gtest dependency — self-contained assert mains) ---
enable_testing()

function(add_trajopt_test name path)
  add_executable(${name} ${path})
  target_link_libraries(${name} PRIVATE
    mrs_trajopt_core mrs_trajopt_plugins mrs_trajopt_adapter mrs_trajopt_io)
  add_test(NAME ${name} COMMAND ${name})
endfunction()

# core unit tests
add_trajopt_test(test_three_clock_gate      test/core/test_three_clock_gate.cpp)
add_trajopt_test(test_velocity_profiler      test/core/test_velocity_profiler.cpp)
add_trajopt_test(test_round_ledger           test/core/test_round_ledger.cpp)
add_trajopt_test(test_safety_monitor         test/core/test_safety_monitor.cpp)
add_trajopt_test(test_startup_checks         test/core/test_startup_checks.cpp)
add_trajopt_test(test_declaration_regulator  test/core/test_declaration_regulator.cpp)
add_trajopt_test(test_control_tick_service   test/core/test_control_tick_service.cpp)
# boundary-artifact checker test (emits JSON; the python checker validates it)
add_trajopt_test(test_boundary_emit          test/core/test_boundary_emit.cpp)
# adapter tests
add_trajopt_test(test_trajectories_roundtrip test/adapter/test_trajectories_adapter_roundtrip.cpp)
add_trajopt_test(test_robot_specs_adapter    test/adapter/test_robot_specs_adapter.cpp)

# =====================================================================================
# ROS WRAPPER (DISABLED — enable on a ROS 2 machine in the integration round).
# The node/ layer (src/node/trajopt_node.cpp) is a skeleton only and is NOT built here.
# When ament is available, uncomment and add ament_target_dependencies for rclcpp,
# mrs_msgs, pluginlib. See 20d "다음 단계 전달 사항 → integration-developer".
# =====================================================================================
# find_package(ament_cmake REQUIRED)
# find_package(rclcpp REQUIRED)
# find_package(mrs_msgs REQUIRED)
# find_package(pluginlib REQUIRED)
# add_executable(trajopt_node src/node/trajopt_node.cpp)
# ament_target_dependencies(trajopt_node rclcpp mrs_msgs pluginlib)
# target_link_libraries(trajopt_node mrs_trajopt_core mrs_trajopt_plugins mrs_trajopt_adapter)
# ament_package()
