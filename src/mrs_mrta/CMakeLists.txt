cmake_minimum_required(VERSION 3.16)
project(mrs_mrta CXX)

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

# --- core (CN-2/CN-3): pure algorithm. No ROS, no rosidl, no JSON, no mrs_msgs.
#     mrs_core::mrs_core_pure is header-only vocabulary (Status/Result/
#     ContractViolation) — it does not itself link ROS/JSON, so this stays
#     within CN-2/CN-3. ---
add_library(mrs_mrta_core STATIC
  src/core/types.cpp)
target_include_directories(mrs_mrta_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(mrs_mrta_core PUBLIC mrs_core::mrs_core_pure)

# --- service (Application, CN-23): drain_once, OverrideLatch, AssignerFactory.
#     No ROS/JSON either (12a §1: "service/ ROS 의존: 없음"). Links core only. ---
add_library(mrs_mrta_service STATIC
  src/service/assignment_service.cpp
  src/service/override_latch.cpp
  src/service/assigner_factory.cpp)
target_link_libraries(mrs_mrta_service PUBLIC mrs_mrta_core)

# --- plugins (CN-5): swap-target implementations. Link core only. ---
add_library(mrs_mrta_plugins STATIC
  src/plugins/assigner_impls.cpp)
target_link_libraries(mrs_mrta_plugins PUBLIC mrs_mrta_core)

# --- adapter (CN-8/CN-9): internal <-> boundary structs. No JSON. ---
add_library(mrs_mrta_adapter STATIC
  src/adapter/assignment_adapter.cpp
  src/adapter/task_release_adapter.cpp
  src/adapter/goal_completion_adapter.cpp
  src/adapter/operator_command_adapter.cpp
  src/adapter/roadmap_adapter.cpp)
target_link_libraries(mrs_mrta_adapter PUBLIC mrs_mrta_core)

# --- tests (CTest; self-contained assert mains, no gtest dependency — 20d 선례) ---
enable_testing()
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/test)  # test_util.hpp, shared by test/{core,adapter}

function(add_mrta_test name path)
  add_executable(${name} ${path})
  target_link_libraries(${name} PRIVATE
    mrs_mrta_core mrs_mrta_service mrs_mrta_plugins mrs_mrta_adapter)
  add_test(NAME ${name} COMMAND ${name})
endfunction()

# core unit tests
add_mrta_test(test_assignment_state      test/core/test_assignment_state.cpp)
add_mrta_test(test_greedy_tail_append    test/core/test_greedy_tail_append.cpp)
add_mrta_test(test_assignment_service    test/core/test_assignment_service.cpp)
# boundary-artifact emission (checker input — validated externally by the
# python checkers, ctest only confirms the emitter itself runs clean)
add_mrta_test(test_boundary_emit         test/core/test_boundary_emit.cpp)
# adapter tests
add_mrta_test(test_assignment_roundtrip  test/adapter/test_assignment_adapter_roundtrip.cpp)
add_mrta_test(test_input_adapters        test/adapter/test_input_adapters.cpp)

# node/ tests — 371 fix. `DeferredQueue<T>` (deferred_release_queue.hpp) has
# zero ROS/mrs_mrta-library dependency by design (file doc: testable without
# rclcpp), so this target only needs the include path, not
# mrs_mrta_core/service/plugins/adapter or ROS.
add_executable(test_deferred_release_queue test/node/test_deferred_release_queue.cpp)
add_test(NAME test_deferred_release_queue COMMAND test_deferred_release_queue)

# =====================================================================================
# ROS node (47차 통합 P2). Statically links core+service+plugins+adapter — NOT a real
# pluginlib::ClassLoader (see src/node/mrta_node.cpp file doc: assigner_impls.cpp has
# zero PLUGINLIB_EXPORT_CLASS calls, out of this round's write scope). plugins.xml is
# still exported so the ament index entry exists for a future real ClassLoader swap.
# =====================================================================================
find_package(rclcpp REQUIRED)
find_package(mrs_msgs REQUIRED)
find_package(pluginlib REQUIRED)

add_executable(mrta_node src/node/mrta_node.cpp)
ament_target_dependencies(mrta_node rclcpp mrs_msgs pluginlib)
target_link_libraries(mrta_node
  mrs_mrta_core mrs_mrta_service mrs_mrta_plugins mrs_mrta_adapter mrs_core::mrs_core_msgs)
pluginlib_export_plugin_description_file(mrs_mrta plugins.xml)

install(TARGETS mrta_node DESTINATION lib/${PROJECT_NAME})

ament_package()
