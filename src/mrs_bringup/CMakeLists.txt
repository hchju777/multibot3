cmake_minimum_required(VERSION 3.16)
project(mrs_bringup)

# 소스 0줄(10_architecture.md:113 · 357§7-2). launch·config만 설치한다.

find_package(ament_cmake REQUIRED)

install(DIRECTORY launch config DESTINATION share/${PROJECT_NAME})

# --- test (F48-4, 48차 코드 게이트) — launch 파일이 실제로 파싱되는지 확인.
# 시험 코드도 "소스 0줄"의 대상이 아니다(347§6-1 항목5: "런치는 소스가 아니다"와
# 같은 이유 — 검증 코드는 파이프라인 구현이 아니다).
if(BUILD_TESTING)
  find_package(ament_cmake_pytest REQUIRED)
  ament_add_pytest_test(test_pipeline_launch_parses test/test_pipeline_launch_parses.py)
endif()

ament_package()
