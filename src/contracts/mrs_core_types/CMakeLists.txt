cmake_minimum_required(VERSION 3.28)
project(mrs_core_types)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()
add_compile_options(-Wall -Wextra -Wpedantic -Werror)

find_package(ament_cmake REQUIRED)

# 헤더 전용 패키지 — 링크할 라이브러리가 없다. INTERFACE 타깃으로 include 경로만 내보낸다.
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include>")
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

install(DIRECTORY include/ DESTINATION include)
install(TARGETS ${PROJECT_NAME} EXPORT export_${PROJECT_NAME})

ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)
ament_export_include_directories(include)

if(BUILD_TESTING)
  # unit 티어 규약(architecture §6 / nav2-reference §2-G2): 이 테스트들은 **rclcpp 에 링크하지
  # 않는다.** 헤더 전용 패키지이므로 링크 대상 자체가 gtest 뿐이다.
  find_package(ament_cmake_gtest REQUIRED)

  # ── 런타임 유닛 테스트 ──────────────────────────────────────────────────────
  ament_add_gtest(test_mrs_core_types
    test/unit/test_view_ids.cpp
    test/unit/test_startup_checks.cpp
    test/unit/test_contract_types.cpp)
  if(TARGET test_mrs_core_types)
    target_link_libraries(test_mrs_core_types ${PROJECT_NAME})
  endif()

  # ── 부정 컴파일 테스트 (컴파일 실패를 기대하는 회귀) ────────────────────────
  # 런타임 테스트로는 "컴파일이 거부되는가"를 검증할 수 없다. 뷰 강타입의 벽이 무너져도 정상
  # 경로 테스트는 전부 통과하므로 별도 방어선이 필요하다.
  get_filename_component(MRS_REPO_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../.." ABSOLUTE)
  if(NOT EXISTS "${MRS_REPO_ROOT}/tools/mrs_negative_compile.cmake")
    message(FATAL_ERROR "부정 컴파일 헬퍼를 찾지 못했다 — ${MRS_REPO_ROOT}/tools/mrs_negative_compile.cmake")
  endif()
  include("${MRS_REPO_ROOT}/tools/mrs_negative_compile.cmake")

  set(MRS_CORE_TYPES_NEGATIVE_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/test/negative/view_ids_negative.cpp")
  set(MRS_CORE_TYPES_NEGATIVE_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/include")

  foreach(negative_case
      CASE_CROSS_VIEW_ASSIGN
      CASE_STRONG_TO_RAW_IMPLICIT
      CASE_RAW_TO_STRONG_IMPLICIT
      CASE_WRONG_VIEW_IN_WINDOW_SEGMENT
      CASE_WRONG_VIEW_ARRAY
      CASE_CROSS_VIEW_COMPARE)
    string(TOLOWER "${negative_case}" negative_case_lower)
    mrs_add_compile_expectation(neg_compile_view_ids_${negative_case_lower}
      SOURCE "${MRS_CORE_TYPES_NEGATIVE_SOURCE}"
      CASE "${negative_case}"
      EXPECT FAIL
      INCLUDE_DIRS "${MRS_CORE_TYPES_NEGATIVE_INCLUDES}")
  endforeach()

  # 검사 장치 유효성 확인 (오케스트레이터 R-17 표준) — 통과해야 할 정상 코드가 같은 컴파일러·
  # 같은 include 환경에서 실제로 컴파일되는지. 이것이 없으면 위 6건은 공허하게 참일 수 있다.
  mrs_add_compile_expectation(neg_compile_view_ids_control_must_compile
    SOURCE "${MRS_CORE_TYPES_NEGATIVE_SOURCE}"
    CASE "CASE_OK"
    EXPECT PASS
    INCLUDE_DIRS "${MRS_CORE_TYPES_NEGATIVE_INCLUDES}")
endif()

ament_package()
