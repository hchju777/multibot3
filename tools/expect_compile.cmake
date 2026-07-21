# expect_compile.cmake — "이 코드가 컴파일되면 안 된다"를 ctest 테스트로 만드는 스크립트.
#
# 왜 필요한가
#   런타임 gtest 는 "잘못된 사용이 거부되는가"를 검증할 수 없다. 뷰 강타입(view_ids.hpp)과
#   node_id_* 헬퍼의 `static_assert` 는 **컴파일 단계에서만** 작동하는 벽이며, 그 벽이 무너져도
#   정상 경로 테스트는 전부 통과한다. 그래서 컴파일러를 직접 불러 종료코드를 판정한다.
#
# 공허하게 참이 되지 않게 하는 장치 (오케스트레이터 R-17 표준: "검사 장치를 만들면 그 장치가
# 실제로 잡는지 확인한다")
#   1. MRS_EXPECT=PASS 대조군 — 통과해야 할 정상 코드가 **같은 컴파일러·같은 include 환경**으로
#      실제로 컴파일되는지 검사한다. 컴파일러 호출 자체가 깨져 있으면 이 대조군이 먼저 실패하므로,
#      "무엇을 넣어도 거부됨"을 "벽이 작동함"으로 오판하는 경로가 닫힌다.
#   2. 하네스 고장 패턴 선별 — 헤더 미발견·컴파일러 미발견처럼 **벽과 무관한 이유**로 실패한
#      경우는 통과가 아니라 FATAL_ERROR 로 떨어뜨린다. 거부 사유를 묻지 않으면 include 경로 오타가
#      곧 초록불이 된다.
#
# 사용 (-D 로 주입, -P 로 실행):
#   cmake -DMRS_CXX_COMPILER=<컴파일러 절대경로>   # ${CMAKE_CXX_COMPILER} — 하드코딩 금지
#         -DMRS_SOURCE=<검사할 .cpp 절대경로>
#         -DMRS_CASE=<정의할 매크로 이름 또는 빈 문자열>
#         -DMRS_EXPECT=FAIL|PASS
#         -DMRS_INCLUDE_DIRS="<;구분 include 경로 목록>"
#         -DMRS_CXX_STANDARD=<17 등>
#         -DMRS_WORK_DIR=<오브젝트 파일을 버릴 디렉터리>
#         -P expect_compile.cmake
#
# 종료코드: 0 = 기대대로, 1 = 기대와 다름(FATAL_ERROR 메시지에 컴파일러 출력 포함).

cmake_minimum_required(VERSION 3.28)

foreach(required_var MRS_CXX_COMPILER MRS_SOURCE MRS_EXPECT MRS_WORK_DIR)
  if(NOT DEFINED ${required_var})
    message(FATAL_ERROR "expect_compile.cmake: 필수 인자 ${required_var} 가 없다")
  endif()
endforeach()

if(NOT MRS_EXPECT STREQUAL "FAIL" AND NOT MRS_EXPECT STREQUAL "PASS")
  message(FATAL_ERROR "expect_compile.cmake: MRS_EXPECT 는 FAIL 또는 PASS 여야 한다 (받은 값: ${MRS_EXPECT})")
endif()

if(NOT EXISTS "${MRS_SOURCE}")
  message(FATAL_ERROR "expect_compile.cmake: 검사 대상 소스가 없다 — ${MRS_SOURCE}")
endif()

if(NOT DEFINED MRS_CXX_STANDARD)
  set(MRS_CXX_STANDARD 17)
endif()

file(MAKE_DIRECTORY "${MRS_WORK_DIR}")

# 매크로 이름을 오브젝트 파일 이름에 넣어 병렬 실행 시 서로 덮어쓰지 않게 한다.
set(case_tag "${MRS_CASE}")
if(case_tag STREQUAL "")
  set(case_tag "nocase")
endif()
get_filename_component(source_stem "${MRS_SOURCE}" NAME_WE)
set(object_file "${MRS_WORK_DIR}/${source_stem}.${case_tag}.o")

set(compile_args "-std=c++${MRS_CXX_STANDARD}" "-c" "${MRS_SOURCE}" "-o" "${object_file}")

# -Werror 를 의도적으로 넣지 않는다: 부정 테스트는 **경고가 아니라 에러**로 거부되어야 한다.
# 경고를 에러로 승격해 두면 벽이 사라져도 무관한 경고 하나가 "거부됨"을 위조할 수 있다.

set(include_dirs "")
if(DEFINED MRS_INCLUDE_DIRS)
  list(APPEND include_dirs ${MRS_INCLUDE_DIRS})
endif()

# 의존이 깊은 패키지(rosidl 생성 헤더 사슬 등)는 include 경로를 손으로 나열할 수 없다. 그런
# 경우 호출자가 CMake 의 file(GENERATE) 로 **해소된** 경로 목록을 파일에 적어 넘긴다 —
# 목록을 테스트 쪽에서 재구성하면 실제 빌드와 어긋나 하네스가 조용히 고장난다.
if(DEFINED MRS_INCLUDE_DIRS_FILE AND NOT MRS_INCLUDE_DIRS_FILE STREQUAL "")
  if(NOT EXISTS "${MRS_INCLUDE_DIRS_FILE}")
    message(FATAL_ERROR
      "expect_compile.cmake: 하네스 고장 — include 목록 파일이 없다: ${MRS_INCLUDE_DIRS_FILE}")
  endif()
  file(STRINGS "${MRS_INCLUDE_DIRS_FILE}" generated_include_dirs)
  list(APPEND include_dirs ${generated_include_dirs})
endif()

list(REMOVE_DUPLICATES include_dirs)
foreach(include_dir IN LISTS include_dirs)
  if(NOT include_dir STREQUAL "")
    list(APPEND compile_args "-I${include_dir}")
  endif()
endforeach()

if(NOT MRS_CASE STREQUAL "")
  list(APPEND compile_args "-D${MRS_CASE}")
endif()

execute_process(
  COMMAND "${MRS_CXX_COMPILER}" ${compile_args}
  RESULT_VARIABLE compile_result
  OUTPUT_VARIABLE compile_stdout
  ERROR_VARIABLE compile_stderr)

set(compile_output "${compile_stdout}${compile_stderr}")
file(REMOVE "${object_file}")

# ── 하네스 고장 선별 ────────────────────────────────────────────────────────────
# 아래 패턴들은 "벽이 잡았다"가 아니라 "검사 장치가 고장났다"는 뜻이다. 통과로 세면 안 된다.
if(compile_output MATCHES "No such file or directory" OR
   compile_output MATCHES "file not found" OR
   compile_output MATCHES "not found[\r\n]")
  message(FATAL_ERROR
    "expect_compile.cmake: 하네스 고장 — 헤더/파일을 찾지 못했다. 이것은 부정 테스트의 통과 사유가 "
    "될 수 없다 (include 경로를 고쳐라).\n"
    "  case    : ${MRS_CASE}\n"
    "  source  : ${MRS_SOURCE}\n"
    "  compiler: ${MRS_CXX_COMPILER}\n"
    "--- 컴파일러 출력 ---\n${compile_output}")
endif()

# ── 기대 판정 ───────────────────────────────────────────────────────────────────
if(MRS_EXPECT STREQUAL "FAIL")
  if(compile_result EQUAL 0)
    message(FATAL_ERROR
      "expect_compile.cmake: **거부되어야 할 코드가 컴파일됐다** — 이 케이스가 지키던 벽이 무너졌다.\n"
      "  case   : ${MRS_CASE}\n"
      "  source : ${MRS_SOURCE}")
  endif()
  message(STATUS "expect_compile: [${MRS_CASE}] 기대대로 컴파일 거부됨")
else()
  if(NOT compile_result EQUAL 0)
    message(FATAL_ERROR
      "expect_compile.cmake: **통과해야 할 정상 코드가 컴파일되지 않았다** — 부정 테스트 전체가 "
      "공허하게 참일 수 있으므로 이 실패를 무시하지 마라.\n"
      "  case   : ${MRS_CASE}\n"
      "  source : ${MRS_SOURCE}\n"
      "--- 컴파일러 출력 ---\n${compile_output}")
  endif()
  message(STATUS "expect_compile: [${MRS_CASE}] 대조군 정상 컴파일 — 컴파일러 호출 경로 건전")
endif()
