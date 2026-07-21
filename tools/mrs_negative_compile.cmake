# mrs_negative_compile.cmake — 부정 컴파일 테스트를 ctest 에 등록하는 헬퍼.
#
# 패키지 CMakeLists 의 if(BUILD_TESTING) 블록에서 include 해 쓴다. 실제 판정은
# tools/expect_compile.cmake 가 하며(별도 cmake -P 프로세스), 이 파일은 등록만 한다.
#
# 컴파일러·표준·include 경로를 **하드코딩하지 않는다** — 전부 호출 시점의 CMake 변수와
# 타깃 속성에서 가져온다.

if(DEFINED MRS_NEGATIVE_COMPILE_INCLUDED)
  return()
endif()
set(MRS_NEGATIVE_COMPILE_INCLUDED TRUE)

set(MRS_EXPECT_COMPILE_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/expect_compile.cmake")
if(NOT EXISTS "${MRS_EXPECT_COMPILE_SCRIPT}")
  message(FATAL_ERROR "mrs_negative_compile.cmake: expect_compile.cmake 를 찾지 못했다 — ${MRS_EXPECT_COMPILE_SCRIPT}")
endif()

#[[
  mrs_add_compile_expectation(<test_name>
    SOURCE       <검사할 .cpp 절대경로>
    CASE         <정의할 매크로 이름>          # 생략 가능
    EXPECT       FAIL|PASS
    INCLUDE_DIRS <include 경로 목록>)          # 생성자 표현식 허용

  <test_name> 이름의 ctest 테스트 1개를 등록한다.
    EXPECT FAIL — 그 코드가 컴파일되면 테스트 실패(= 지키던 벽이 무너졌다는 뜻).
    EXPECT PASS — 그 코드가 컴파일되지 않으면 테스트 실패(= 부정 테스트가 공허하게 참일 수 있다).
]]
function(mrs_add_compile_expectation test_name)
  cmake_parse_arguments(ARG "" "SOURCE;CASE;EXPECT;INCLUDE_DIRS_FILE" "INCLUDE_DIRS" ${ARGN})

  if(NOT ARG_SOURCE)
    message(FATAL_ERROR "mrs_add_compile_expectation(${test_name}): SOURCE 가 없다")
  endif()
  if(NOT ARG_EXPECT)
    message(FATAL_ERROR "mrs_add_compile_expectation(${test_name}): EXPECT 가 없다")
  endif()
  if(NOT DEFINED ARG_CASE)
    set(ARG_CASE "")
  endif()

  # 리스트를 세미콜론 그대로 넘기면 -D 인자에서 쪼개진다. "|" 로 이어 붙인 뒤 스크립트에서
  # 되돌리는 대신, CMake 가 이해하는 세미콜론 이스케이프를 쓴다.
  string(REPLACE ";" "\\;" escaped_includes "${ARG_INCLUDE_DIRS}")

  set(standard "${CMAKE_CXX_STANDARD}")
  if(NOT standard)
    set(standard 17)
  endif()

  add_test(
    NAME ${test_name}
    COMMAND "${CMAKE_COMMAND}"
      "-DMRS_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
      "-DMRS_SOURCE=${ARG_SOURCE}"
      "-DMRS_CASE=${ARG_CASE}"
      "-DMRS_EXPECT=${ARG_EXPECT}"
      "-DMRS_CXX_STANDARD=${standard}"
      "-DMRS_INCLUDE_DIRS=${escaped_includes}"
      "-DMRS_INCLUDE_DIRS_FILE=${ARG_INCLUDE_DIRS_FILE}"
      "-DMRS_WORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/negative_compile"
      -P "${MRS_EXPECT_COMPILE_SCRIPT}")
endfunction()
