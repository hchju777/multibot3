cmake_minimum_required(VERSION 3.28)
project(mrs_ros_plan_tick)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()
add_compile_options(-Wall -Wextra -Wpedantic -Werror)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(mrs_interfaces REQUIRED)
# mrs_msg_convert: R-18 구속(시각 변환 헬퍼 경유). L2 변환 패키지이지 도메인 패키지가 아니다.
find_package(mrs_msg_convert REQUIRED)

# ── 순수 로직(발행 판정) — ROS 무의존 ──────────────────────────────────────────────────
# R-20 정정("발행도 시계의 함수")의 알맹이는 `TickScheduler` 다. rclcpp 에 닿지 않는 별도
# 타깃으로 떼어 두는 이유는 하나다: **unit 티어에서 검증 가능하게 하기 위해서**다
# (architecture §6 / tools/check_boundaries.sh 검사 [6][7] — 테스트는 rclcpp 를 링크하지 않는다).
# ⚠ GLOB 을 쓰지 않고 소스를 명시한다. GLOB 이면 이 파일이 실행 파일에도 함께 들어가
#   같은 심볼이 두 번 정의된다.
add_library(${PROJECT_NAME}_logic STATIC src/tick_scheduler.cpp)
target_include_directories(${PROJECT_NAME}_logic PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include>")
target_compile_features(${PROJECT_NAME}_logic PUBLIC cxx_std_17)

add_executable(plan_tick_node_exe src/main.cpp src/plan_tick_node.cpp)
target_include_directories(plan_tick_node_exe PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include>")
target_compile_features(plan_tick_node_exe PUBLIC cxx_std_17)
# ⚠ 키워드 시그니처(PUBLIC)로 통일한다. `ament_target_dependencies(... PUBLIC ...)` 가 키워드
#   형태를 쓰므로, 여기서 평문 형태를 쓰면 CMake 가 같은 타깃에 두 형태 혼용을 거부한다.
target_link_libraries(plan_tick_node_exe PUBLIC ${PROJECT_NAME}_logic)
ament_target_dependencies(plan_tick_node_exe PUBLIC rclcpp mrs_interfaces mrs_msg_convert)

install(TARGETS plan_tick_node_exe DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include)

if(BUILD_TESTING)
  # unit 티어 규약(architecture §6 / nav2-reference §2-G2): 이 테스트 바이너리는 **rclcpp 에
  # 링크하지 않는다.** 링크하는 순간 unit 티어가 조용히 pysim 티어로 승격되고, 결정론적으로
  # 빠르게 도는 방어선이 사라진다. 위반 감시는 tools/check_boundaries.sh 검사 [6]/[7] 이 맡는다.
  find_package(ament_cmake_gtest REQUIRED)

  ament_add_gtest(test_mrs_ros_plan_tick test/unit/test_tick_scheduler.cpp)
  if(TARGET test_mrs_ros_plan_tick)
    target_link_libraries(test_mrs_ros_plan_tick ${PROJECT_NAME}_logic)
  endif()
endif()

ament_package()
