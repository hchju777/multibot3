cmake_minimum_required(VERSION 3.28)
project(mrs_map_registry)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()
add_compile_options(-Wall -Wextra -Wpedantic -Werror)

find_package(ament_cmake REQUIRED)
find_package(mrs_core_types REQUIRED)
find_package(yaml-cpp REQUIRED)

file(GLOB_RECURSE MRS_MAP_REGISTRY_SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_library(${PROJECT_NAME} ${MRS_MAP_REGISTRY_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include>")
# 내부 헤더(src/map_registry_impl.hpp)는 같은 디렉터리의 .cpp 만 인용한다(따옴표 인클루드) — 설치·export 하지 않는다.
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
ament_target_dependencies(${PROJECT_NAME} PUBLIC mrs_core_types)
# yaml-cpp 는 로더 TU(map_registry_loader.cpp) 내부 전용 — 공개 헤더는 yaml 를 include 하지 않는다.
# 다만 정적 라이브러리라 이 링크 의존은 export 에 전파되므로, 하류가 imported 타깃을 해소할 수 있게
# ament_export_dependencies 로 내보낸다(코드/헤더 노출 아님 — 전이 링크 의존일 뿐).
target_link_libraries(${PROJECT_NAME} PRIVATE yaml-cpp::yaml-cpp)

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
ament_export_dependencies(mrs_core_types yaml-cpp)


if(BUILD_TESTING)
  # unit 티어 회귀 테스트 (test-author). 공개 헤더 map_registry.hpp 만 include 하고
  # 내부 src/map_registry_impl.hpp 는 인용하지 않는다 — 파사드 계약만 검증한다.
  # rclcpp 에 링크하지 않는다(순수 도메인 패키지, unit 티어 규약).
  find_package(ament_cmake_gtest REQUIRED)

  ament_add_gtest(test_mrs_map_registry
    test/unit/test_map_loader.cpp
    test/unit/test_map_validators.cpp
    test/unit/test_map_subdivision.cpp
    test/unit/test_map_transform.cpp)
  if(TARGET test_mrs_map_registry)
    target_link_libraries(test_mrs_map_registry ${PROJECT_NAME})
    # 공용 픽스처 헤더(test/unit/map_registry_test_fixtures.hpp) 인용 경로.
    target_include_directories(test_mrs_map_registry PRIVATE
      "${CMAKE_CURRENT_SOURCE_DIR}/test/unit")
  endif()
endif()

ament_package()
