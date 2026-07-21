cmake_minimum_required(VERSION 3.28)
project(mrs_sim_abstraction)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()
add_compile_options(-Wall -Wextra -Wpedantic -Werror)

find_package(ament_cmake REQUIRED)
find_package(mrs_core_types REQUIRED)

file(GLOB_RECURSE MRS_SIM_ABSTRACTION_SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_library(${PROJECT_NAME} ${MRS_SIM_ABSTRACTION_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include>")
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
ament_target_dependencies(${PROJECT_NAME} PUBLIC mrs_core_types)

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
ament_export_dependencies(mrs_core_types)


if(BUILD_TESTING)
  # unit 티어 규약(architecture §6): 도메인 패키지이므로 rclcpp·mrs_interfaces 에 링크하지
  # 않는다(C1). 이 테스트는 sleep 도 쓰지 않으며 시간은 step() 으로만 전진한다.
  #
  # ⚠ 이 테스트들의 통과는 **물리 검증이 아니다** — FakeSimBackend 는 PhysicsFidelity::NONE
  #   이며 QP feasibility·livelock·물리 재현성은 isaac 필수다(architecture §5.3).
  find_package(ament_cmake_gtest REQUIRED)

  ament_add_gtest(test_mrs_sim_abstraction test/unit/test_fake_sim_backend.cpp)
  if(TARGET test_mrs_sim_abstraction)
    target_link_libraries(test_mrs_sim_abstraction ${PROJECT_NAME})
  endif()
endif()

ament_package()
