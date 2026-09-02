cmake_minimum_required(VERSION 3.16)
project(mrs_mapf CXX)

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
add_library(mrs_mapf_core STATIC
  src/core/segment_graph.cpp
  src/core/self_checks.cpp
  src/core/reservation.cpp
  src/core/visit_order.cpp)
target_include_directories(mrs_mapf_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(mrs_mapf_core PUBLIC mrs_core::mrs_core_pure)

# --- plugins (CN-5): swap-target implementations. Link core only. ---
add_library(mrs_mapf_plugins STATIC
  src/plugins/priority_safe_interval_search.cpp)
target_link_libraries(mrs_mapf_plugins PUBLIC mrs_mapf_core)

# --- adapter (CN-8/CN-9): internal <-> boundary structs. No JSON. ---
add_library(mrs_mapf_adapter STATIC
  src/adapter/roadmap_adapter.cpp
  src/adapter/assignment_adapter.cpp
  src/adapter/replan_request_adapter.cpp
  src/adapter/robot_specs_adapter.cpp
  src/adapter/discrete_plan_adapter.cpp)
target_link_libraries(mrs_mapf_adapter PUBLIC mrs_mapf_core)

# --- io: JSON (de)serialization. Kept out of core/adapter (CN-2/CN-3). ---
add_library(mrs_mapf_io STATIC
  src/io/json_io.cpp)
target_link_libraries(mrs_mapf_io PUBLIC mrs_mapf_adapter)

# --- service (Application layer, CN-23): the wrapping procedure. Links core+plugins. ---
add_library(mrs_mapf_service STATIC
  src/service/mapf_planning_service.cpp)
target_link_libraries(mrs_mapf_service PUBLIC mrs_mapf_core mrs_mapf_plugins)

# --- tests (CTest; self-contained assert mains — no gtest dependency) ---
enable_testing()

function(add_mapf_test name path)
  add_executable(${name} ${path})
  target_link_libraries(${name} PRIVATE
    mrs_mapf_core mrs_mapf_plugins mrs_mapf_adapter mrs_mapf_io mrs_mapf_service)
  add_test(NAME ${name} COMMAND ${name})
endfunction()

# core unit tests
add_mapf_test(test_self_checks             test/core/test_self_checks.cpp)
add_mapf_test(test_segment_graph           test/core/test_segment_graph.cpp)
add_mapf_test(test_reservation_rb1         test/core/test_reservation_rb1.cpp)
add_mapf_test(test_priority_safe_interval  test/core/test_priority_safe_interval_search.cpp)
add_mapf_test(test_visit_order             test/core/test_visit_order.cpp)
add_mapf_test(test_planning_service        test/core/test_planning_service.cpp)
# boundary-artifact checker test (emits JSON; the python checkers validate it)
add_mapf_test(test_boundary_emit           test/core/test_boundary_emit.cpp)
# adapter tests
add_mapf_test(test_discrete_plan_roundtrip test/adapter/test_discrete_plan_adapter_roundtrip.cpp)
add_mapf_test(test_roadmap_adapter         test/adapter/test_roadmap_adapter.cpp)
add_mapf_test(test_replan_request_adapter  test/adapter/test_replan_request_adapter.cpp)
# envelope-version guard (함정 ③ — diffs the schema file's real version)
add_mapf_test(test_check_envelope_version  test/core/test_check_envelope_version.cpp)
target_compile_definitions(test_check_envelope_version PRIVATE
  MRS_MAPF_SCHEMA_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../mrs_msgs/schema/discrete_plan.schema.json")
# node-layer orchestration test (pure — no rclcpp): 48차 fix for the
# "revision advances but plan_revision sticks at 0" bug (mapf_node.cpp:204).
add_mapf_test(test_replan_trigger          test/node/test_replan_trigger.cpp)

# =====================================================================================
# ROS node (47차 통합 P2). Statically links core+plugins+adapter+service — this package
# has no *Factory/ClassLoader (only one concrete IPlanningStrategy exists this round).
# =====================================================================================
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(mrs_msgs REQUIRED)
find_package(pluginlib REQUIRED)

add_executable(mapf_node src/node/mapf_node.cpp)
ament_target_dependencies(mapf_node rclcpp rclcpp_action mrs_msgs pluginlib)
target_link_libraries(mapf_node
  mrs_mapf_core mrs_mapf_plugins mrs_mapf_adapter mrs_mapf_service mrs_core::mrs_core_msgs)
# 🔴 declarative only (D3/D4 debt closure) — mapf_node does NOT use a real
# pluginlib::ClassLoader (see node/mapf_node.cpp file doc: only one concrete
# IPlanningStrategy exists this round, statically linked instead).
pluginlib_export_plugin_description_file(mrs_mapf plugins.xml)

install(TARGETS mapf_node DESTINATION lib/${PROJECT_NAME})

ament_package()
