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
  # gtest 골격은 Phase 5(test-author)가 내용과 함께 생성한다 (design-decision v5 build_policy).
  # 빈 테스트 파일을 두지 않는다 — 자리만 남긴다.
endif()

ament_package()
