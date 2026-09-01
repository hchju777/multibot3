cmake_minimum_required(VERSION 3.16)
project(mrs_sadg CXX)

# 🔴 ROS 2 런타임이 이 체크아웃에 없다. 그래서 이 CMakeLists는 순수 CMake로
#    코어 라이브러리 + 시험을 빌드한다. ament/rclcpp/rosidl 절은 통합 라운드에서
#    ROS 머신으로 옮길 때 켠다 — 아래 "ROS WRAPPER (DISABLED)" 블록 참조.
#    (선례: mrs_mapf/CMakeLists.txt · mrs_trajopt/CMakeLists.txt — 같은 패턴)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Wpedantic)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# --- core (CN-2/CN-3): pure algorithm. No ROS, no rosidl, no JSON, no mrs_msgs. ---
add_library(mrs_sadg_core STATIC
  src/core/execution_graph.cpp
  src/core/extension_types.cpp
  src/core/constraint_compiler.cpp
  src/core/self_checks.cpp
  src/core/standby_resolver.cpp
  src/core/safety_envelope.cpp
  src/core/commit_gate.cpp
  src/core/commit_arbiter.cpp
  src/core/release_checker.cpp
  src/core/progress_deficit.cpp
  src/core/instr_ring.cpp)
target_include_directories(mrs_sadg_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# --- plugins (CN-5): swap-target implementations. Link core only. ---
add_library(mrs_sadg_plugins STATIC
  src/plugins/alt_head_lift.cpp
  src/plugins/alt_adjacent_swap.cpp
  src/plugins/layer_branch_bound.cpp
  src/plugins/layer_fixed_order.cpp
  src/plugins/policy_deficit_ladder.cpp
  src/plugins/policy_constant_rung.cpp)
target_link_libraries(mrs_sadg_plugins PUBLIC mrs_sadg_core)

# --- adapter (CN-8/CN-9): internal <-> boundary structs. No JSON. ---
add_library(mrs_sadg_adapter STATIC
  src/adapter/plan_adapter.cpp
  src/adapter/constraints_adapter.cpp)
target_link_libraries(mrs_sadg_adapter PUBLIC mrs_sadg_core)

# --- io: JSON (de)serialization. Kept out of core/adapter (CN-1/CN-2/CN-3). ---
add_library(mrs_sadg_io STATIC
  src/io/json_io.cpp)
target_link_libraries(mrs_sadg_io PUBLIC mrs_sadg_adapter)

# --- service (Application layer, CN-23): the tick + reorder use cases. ---
add_library(mrs_sadg_service STATIC
  src/service/execution_tick_service.cpp
  src/service/order_search_service.cpp)
target_link_libraries(mrs_sadg_service PUBLIC mrs_sadg_core)

# --- tests (CTest; self-contained assert mains — no gtest dependency) ---
enable_testing()

function(add_sadg_test name path)
  add_executable(${name} ${path})
  target_link_libraries(${name} PRIVATE
    mrs_sadg_core mrs_sadg_plugins mrs_sadg_adapter mrs_sadg_io mrs_sadg_service)
  add_test(NAME ${name} COMMAND ${name})
endfunction()

# core unit tests
add_sadg_test(test_constraint_compiler       test/core/test_constraint_compiler.cpp)
add_sadg_test(test_self_checks_seam          test/core/test_self_checks_seam.cpp)
add_sadg_test(test_safety_envelope           test/core/test_safety_envelope.cpp)
add_sadg_test(test_standby_resolver          test/core/test_standby_resolver.cpp)
add_sadg_test(test_execution_tick            test/core/test_execution_tick.cpp)
add_sadg_test(test_escalation_policy         test/core/test_escalation_policy.cpp)
add_sadg_test(test_order_search              test/core/test_order_search.cpp)
add_sadg_test(test_release_checker           test/core/test_release_checker.cpp)
# boundary-artifact checker test (emits JSON; the python checkers validate it)
add_sadg_test(test_boundary_emit             test/core/test_boundary_emit.cpp)
# adapter tests
add_sadg_test(test_plan_adapter_roundtrip    test/adapter/test_plan_adapter_roundtrip.cpp)
add_sadg_test(test_constraints_adapter       test/adapter/test_constraints_adapter_roundtrip.cpp)

# =====================================================================================
# ROS WRAPPER (DISABLED — enable on a ROS 2 machine in the integration round).
# The node/ layer (src/node/{sadg_t0_node,switch_selector_node}.cpp) is a skeleton
# only and is NOT built here. Plugin registration (plugins.xml + pluginlib) also
# lands in the integration round.
# =====================================================================================
# find_package(ament_cmake REQUIRED)
# find_package(rclcpp REQUIRED)
# find_package(rclcpp_action REQUIRED)
# find_package(mrs_msgs REQUIRED)
# find_package(pluginlib REQUIRED)
# add_executable(sadg_t0_node src/node/sadg_t0_node.cpp)
# add_executable(switch_selector_node src/node/switch_selector_node.cpp)
# ament_target_dependencies(sadg_t0_node rclcpp rclcpp_action mrs_msgs pluginlib)
# ament_target_dependencies(switch_selector_node rclcpp rclcpp_action mrs_msgs pluginlib)
# target_link_libraries(sadg_t0_node
#   mrs_sadg_core mrs_sadg_plugins mrs_sadg_adapter mrs_sadg_io mrs_sadg_service)
# target_link_libraries(switch_selector_node
#   mrs_sadg_core mrs_sadg_plugins mrs_sadg_adapter mrs_sadg_io mrs_sadg_service)
# ament_package()
