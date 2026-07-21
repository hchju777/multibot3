cmake_minimum_required(VERSION 3.28)
project(mrs_msg_convert)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()
add_compile_options(-Wall -Wextra -Wpedantic -Werror)

# mrs_reorder_btpg / mrs_reorder_milp / mrs_ladder 는 R-15 (c2) 로 제거했다 — 헤더 include 에
# 대응이 없었다(실측). ⚠ mrs_local_traj 는 실제 사용 중이므로 남긴다.
find_package(ament_cmake REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(mrs_core_types REQUIRED)
find_package(mrs_interfaces REQUIRED)
find_package(mrs_map_registry REQUIRED)
find_package(mrs_planning REQUIRED)
find_package(mrs_depgraph REQUIRED)
find_package(mrs_judge REQUIRED)
find_package(mrs_task_alloc REQUIRED)
find_package(mrs_local_traj REQUIRED)
find_package(mrs_sim_abstraction REQUIRED)
find_package(mrs_metrics REQUIRED)

file(GLOB_RECURSE MRS_MSG_CONVERT_SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_library(${PROJECT_NAME} ${MRS_MSG_CONVERT_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include>")
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
ament_target_dependencies(${PROJECT_NAME} PUBLIC geometry_msgs mrs_core_types mrs_interfaces mrs_map_registry mrs_planning mrs_depgraph mrs_judge mrs_task_alloc mrs_local_traj mrs_sim_abstraction mrs_metrics)

install(DIRECTORY include/ DESTINATION include)
install(
  TARGETS ${PROJECT_NAME}
  EXPORT export_${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)
ament_export_include_directories(include)
ament_export_dependencies(geometry_msgs mrs_core_types mrs_interfaces mrs_map_registry mrs_planning mrs_depgraph mrs_judge mrs_task_alloc mrs_local_traj mrs_sim_abstraction mrs_metrics)


if(BUILD_TESTING)
  # unit 티어 규약(architecture §6 / nav2-reference §2-G2): 이 테스트 바이너리는 **rclcpp 에
  # 링크하지 않는다.** mrs_interfaces 는 rosidl 인터페이스 패키지라 rclcpp::init 없이도
  # 메시지 타입을 쓸 수 있고, 이 테스트들이 정확히 그 사실에 의존한다.
  # 위반 감시는 tools/check_boundaries.sh 검사 [6]/[7] 이 맡는다.
  find_package(ament_cmake_gtest REQUIRED)

  # ── 런타임 유닛 테스트 ──────────────────────────────────────────────────────
  ament_add_gtest(test_mrs_msg_convert
    test/unit/test_common_convert.cpp
    test/unit/test_convert_roundtrip.cpp
    test/unit/test_convert_rejects.cpp
    test/unit/test_convert_out_untouched.cpp)
  if(TARGET test_mrs_msg_convert)
    target_include_directories(test_mrs_msg_convert PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/test/unit")
    target_link_libraries(test_mrs_msg_convert ${PROJECT_NAME})
  endif()

  # ── 부정 컴파일 테스트 ──────────────────────────────────────────────────────
  # node_id_* 헬퍼의 static_assert 벽(규칙 V3)은 컴파일 단계에서만 작동한다. 런타임 테스트로는
  # 그 벽이 살아 있는지 알 수 없으므로 컴파일러를 직접 불러 종료코드를 본다.
  get_filename_component(MRS_REPO_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../.." ABSOLUTE)
  if(NOT EXISTS "${MRS_REPO_ROOT}/tools/mrs_negative_compile.cmake")
    message(FATAL_ERROR "부정 컴파일 헬퍼를 찾지 못했다 — ${MRS_REPO_ROOT}/tools/mrs_negative_compile.cmake")
  endif()
  include("${MRS_REPO_ROOT}/tools/mrs_negative_compile.cmake")

  # include 경로는 **실제 라이브러리 타깃이 쓰는 것과 같은 목록**을 생성 시각에 받아쓴다.
  # 손으로 나열하거나 절대경로를 하드코딩하면 rosidl 생성 헤더 사슬(builtin_interfaces 등)이
  # 빠져 부정 테스트가 "헤더 없음"으로 실패한다 — 실제로 그렇게 한 번 실패시켜 확인했다.
  set(MRS_CONVERT_NEGATIVE_INCLUDE_FILE "${CMAKE_CURRENT_BINARY_DIR}/negative_include_dirs.txt")
  file(GENERATE
    OUTPUT "${MRS_CONVERT_NEGATIVE_INCLUDE_FILE}"
    CONTENT "$<JOIN:$<TARGET_PROPERTY:${PROJECT_NAME},INCLUDE_DIRECTORIES>,\n>\n")

  set(MRS_CONVERT_NEGATIVE_SOURCE
    "${CMAKE_CURRENT_SOURCE_DIR}/test/negative/common_convert_negative.cpp")

  foreach(negative_case
      CASE_N1_INT_LITERAL_UNWRAP
      CASE_N2_BARE_UINT32_UNWRAP
      CASE_N3_BARE_UINT32_OUT
      CASE_N4_BARE_UINT32_ARRAY_UNWRAP
      CASE_N5_BARE_UINT32_ARRAY_OUT
      CASE_N6_CROSS_VIEW_ASSIGN_AFTER_WRAP
      CASE_N7_CROSS_VIEW_ARRAY
      CASE_MIRROR_DRIFT)
    string(TOLOWER "${negative_case}" negative_case_lower)
    mrs_add_compile_expectation(neg_compile_common_convert_${negative_case_lower}
      SOURCE "${MRS_CONVERT_NEGATIVE_SOURCE}"
      CASE "${negative_case}"
      EXPECT FAIL
      INCLUDE_DIRS_FILE "${MRS_CONVERT_NEGATIVE_INCLUDE_FILE}")
  endforeach()

  # 검사 장치 유효성 확인 (R-17 표준). 이 대조군이 실패하면 위 8건의 "거부됨"은 include 경로
  # 고장이 만든 가짜일 수 있으므로, 그 경우 부정 테스트 전체를 신뢰하지 않는다.
  mrs_add_compile_expectation(neg_compile_common_convert_control_must_compile
    SOURCE "${MRS_CONVERT_NEGATIVE_SOURCE}"
    CASE "CASE_OK"
    EXPECT PASS
    INCLUDE_DIRS_FILE "${MRS_CONVERT_NEGATIVE_INCLUDE_FILE}")
endif()

ament_package()
