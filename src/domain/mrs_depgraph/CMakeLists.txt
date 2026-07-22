cmake_minimum_required(VERSION 3.28)
project(mrs_depgraph)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()
add_compile_options(-Wall -Wextra -Wpedantic -Werror)

find_package(ament_cmake REQUIRED)
find_package(mrs_core_types REQUIRED)
find_package(mrs_map_registry REQUIRED)

file(GLOB_RECURSE MRS_DEPGRAPH_SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_library(${PROJECT_NAME} ${MRS_DEPGRAPH_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include>")
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
ament_target_dependencies(${PROJECT_NAME} PUBLIC mrs_core_types mrs_map_registry)

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
ament_export_dependencies(mrs_core_types mrs_map_registry)


if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)

  ament_add_gtest(test_mrs_depgraph
    test/unit/test_depgraph_build.cpp
    test/unit/test_depgraph_release.cpp)
  if(TARGET test_mrs_depgraph)
    target_link_libraries(test_mrs_depgraph ${PROJECT_NAME})
  endif()
endif()

ament_package()
