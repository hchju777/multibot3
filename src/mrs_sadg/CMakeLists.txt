cmake_minimum_required(VERSION 3.16)
project(mrs_sadg CXX)

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
target_link_libraries(mrs_sadg_core PUBLIC mrs_core::mrs_core_pure)

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
# ROS nodes (47차 통합 P2). 🔴 두 노드 모두 축소 배선이다 — 아래 각 target 주석 및
# src/node/*.cpp 파일 주석 참조. Reorder 채널(sadg_t0<->switch_selector)은
# 350_msg_idl_decisions.md §5-2 D3-b가 [결정 부재]로 남겼고 47차 발주서가
# "Reorder.action을 만들지 마라"로 명시했으므로 이번 라운드는 그 배선을 하지 않는다.
# =====================================================================================
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(mrs_msgs REQUIRED)
find_package(pluginlib REQUIRED)

# sadg_t0_node: /discrete_plan 구독 -> ConstraintCompiler -> /execution_constraints
# 발행 경로만 배선했다. ExecutionTickService의 매틱 오케스트레이션(릴리스 비트·승급·
# EVAC·재계획 액션 클라이언트)은 이번 라운드에 배선하지 않았다 — 클록 주입·결손 계산·
# WcInput 구성 등 추가 설계가 필요해 스코프 밖으로 신고한다(21_integration.md).
add_executable(sadg_t0_node src/node/sadg_t0_node.cpp)
ament_target_dependencies(sadg_t0_node rclcpp rclcpp_action mrs_msgs pluginlib)
target_link_libraries(sadg_t0_node
  mrs_sadg_core mrs_sadg_plugins mrs_sadg_adapter mrs_sadg_io mrs_sadg_service
  mrs_core::mrs_core_msgs)

# switch_selector_node: /discrete_plan 구독만 배선했다. Reorder 액션 서버는 위 사유로
# 이번 라운드에 서비스하지 않는다(D3-b 결정 부재 승계).
add_executable(switch_selector_node src/node/switch_selector_node.cpp)
ament_target_dependencies(switch_selector_node rclcpp rclcpp_action mrs_msgs pluginlib)
target_link_libraries(switch_selector_node
  mrs_sadg_core mrs_sadg_plugins mrs_sadg_adapter mrs_sadg_io mrs_sadg_service
  mrs_core::mrs_core_msgs)

# 🔴 declarative only (D3/D4 debt closure) — 두 노드 다 실제 pluginlib::ClassLoader를
# 쓰지 않는다. sadg_t0_node는 ConstraintCompiler 안에서 IAlternativeGenerator 구현체
# (AltHeadLift)를 정적으로 인스턴스화하고, switch_selector_node는 아예 확장점을 로드하지
# 않는다(Reorder 서버 자체가 없다 — 위 주석). `src/plugins/*.cpp`에 `PLUGINLIB_EXPORT_CLASS`
# 등록 매크로가 트리 전체 0건(grep 확인)이고 `mrs_sadg_plugins`는 STATIC 라이브러리라
# `.so` 산출물이 0개다 — 그래서 이 index 등록은 ament가 참조할 대상이 없다(mrs_mapf·
# mrs_trajopt와 같은 형태의 신고).
pluginlib_export_plugin_description_file(mrs_sadg plugins.xml)

install(TARGETS sadg_t0_node switch_selector_node DESTINATION lib/${PROJECT_NAME})

ament_package()
