cmake_minimum_required(VERSION 3.16)
project(mrs_trajopt CXX)

# 47차 통합(P2) — ament 절 해제. core/status.hpp가 이제 mrs_core를 include하므로
# mrs_core는 ROS 유무와 무관하게 항상 필요하다(D15 별칭 스왑).

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Wpedantic)

find_package(ament_cmake REQUIRED)
find_package(mrs_core REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# --- core (CN-2/CN-3): pure algorithm. No ROS, no rosidl, no JSON, no mrs_msgs. ---
add_library(mrs_trajopt_core STATIC
  src/core/velocity_profiler.cpp
  src/core/startup_checks.cpp)
target_include_directories(mrs_trajopt_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(mrs_trajopt_core PUBLIC mrs_core::mrs_core_pure)

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

# --- service (Application layer, CN-23): the three-clock tick use case. ---
add_library(mrs_trajopt_service STATIC
  src/service/control_tick_service.cpp)
target_link_libraries(mrs_trajopt_service PUBLIC mrs_trajopt_core)

# --- measurement spike runner (output c) ---
add_executable(mrs_trajopt_spike spike/measure_spike.cpp)
target_link_libraries(mrs_trajopt_spike PRIVATE
  mrs_trajopt_core mrs_trajopt_plugins mrs_trajopt_adapter mrs_trajopt_io mrs_trajopt_service)

# --- tests (CTest; no gtest dependency — self-contained assert mains) ---
enable_testing()

function(add_trajopt_test name path)
  add_executable(${name} ${path})
  target_link_libraries(${name} PRIVATE
    mrs_trajopt_core mrs_trajopt_plugins mrs_trajopt_adapter mrs_trajopt_io mrs_trajopt_service)
  add_test(NAME ${name} COMMAND ${name})
endfunction()

# core unit tests
add_trajopt_test(test_three_clock_gate      test/core/test_three_clock_gate.cpp)
add_trajopt_test(test_velocity_profiler      test/core/test_velocity_profiler.cpp)
add_trajopt_test(test_round_ledger           test/core/test_round_ledger.cpp)
add_trajopt_test(test_safety_monitor         test/core/test_safety_monitor.cpp)
add_trajopt_test(test_startup_checks         test/core/test_startup_checks.cpp)
add_trajopt_test(test_declaration_regulator  test/core/test_declaration_regulator.cpp)
add_trajopt_test(test_world_observation      test/core/test_world_observation.cpp)
add_trajopt_test(test_declaration_ledger     test/core/test_declaration_ledger.cpp)
add_trajopt_test(test_control_tick_service   test/core/test_control_tick_service.cpp)
# boundary-artifact checker test (emits JSON; the python checker validates it)
add_trajopt_test(test_boundary_emit          test/core/test_boundary_emit.cpp)
# adapter tests
add_trajopt_test(test_trajectories_roundtrip test/adapter/test_trajectories_adapter_roundtrip.cpp)
add_trajopt_test(test_robot_specs_adapter    test/adapter/test_robot_specs_adapter.cpp)

# =====================================================================================
# ROS node (47차 통합 P2). 🔴 SCOPED-DOWN — see src/node/trajopt_node.cpp file doc:
# the full ControlTickService loop needs mrs_sim's odom feed (P4, out of this round).
# =====================================================================================
find_package(rclcpp REQUIRED)
find_package(mrs_msgs REQUIRED)
find_package(pluginlib REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)

add_executable(trajopt_node src/node/trajopt_node.cpp)
ament_target_dependencies(trajopt_node rclcpp mrs_msgs pluginlib geometry_msgs nav_msgs sensor_msgs)
target_link_libraries(trajopt_node
  mrs_trajopt_core mrs_trajopt_plugins mrs_trajopt_adapter mrs_trajopt_service
  mrs_core::mrs_core_msgs)
# 🔴 declarative only (48차 웨이브1-B, 368) — trajopt_node still does NOT use a
# real pluginlib::ClassLoader (no *Factory exists; plugins.xml's 8 classes are
# not PLUGINLIB_EXPORT_CLASS-registered, grep-confirmed — see 361§8 debt for
# mrta's identical situation). This round direct-instantiates two concrete
# plugin classes as owned members (SearchFixedPathGating, SubgoalFreeSpaceDisk)
# instead — see trajopt_node.cpp file doc D2/D3.
pluginlib_export_plugin_description_file(mrs_trajopt plugins.xml)

install(TARGETS trajopt_node DESTINATION lib/${PROJECT_NAME})

ament_package()
