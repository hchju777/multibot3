cmake_minimum_required(VERSION 3.16)
project(mrs_mrta CXX)

# 🔴 ROS 2 런타임이 이 체크아웃에 없다(brief §1). 그래서 이 CMakeLists는 **순수 CMake**로
#    core+service+plugins+adapter(+시험)를 빌드한다. ament/rclcpp/rosidl 절은 통합 라운드에서
#    ROS 머신으로 옮길 때 켠다 — 아래 "ROS WRAPPER (DISABLED)" 블록 참조.

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Wpedantic)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# --- core (CN-2/CN-3): pure algorithm. No ROS, no rosidl, no JSON, no mrs_msgs. ---
add_library(mrs_mrta_core STATIC
  src/core/types.cpp)
target_include_directories(mrs_mrta_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

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

# =====================================================================================
# ROS WRAPPER (DISABLED — enable on a ROS 2 machine in the integration round).
# The node/ layer (src/node/mrta_node.cpp) is a skeleton only and is NOT built here.
# When ament is available, uncomment and add ament_target_dependencies for rclcpp,
# mrs_msgs, pluginlib. See 20a "다음 단계 전달 사항 → integration-developer".
# =====================================================================================
# find_package(ament_cmake REQUIRED)
# find_package(rclcpp REQUIRED)
# find_package(mrs_msgs REQUIRED)
# find_package(pluginlib REQUIRED)
# add_executable(mrta_node src/node/mrta_node.cpp)
# ament_target_dependencies(mrta_node rclcpp mrs_msgs pluginlib)
# target_link_libraries(mrta_node mrs_mrta_core mrs_mrta_service mrs_mrta_plugins mrs_mrta_adapter)
# pluginlib_export_plugin_description_file(mrs_mrta plugins.xml)
# ament_package()
