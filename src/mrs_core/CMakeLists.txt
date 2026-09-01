cmake_minimum_required(VERSION 3.16)
project(mrs_core CXX)

# 47차 P1 — mrs_core 최소(347_arch_integration_delta.md §3-1 D14). 두 타깃(CN-3):
# mrs_core_pure(ROS·JSON·pluginlib 미링크) / mrs_core_msgs(mrs_msgs 링크).

find_package(ament_cmake REQUIRED)
find_package(mrs_msgs REQUIRED)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Wpedantic)

# --- mrs_core_pure (CN-2/CN-3): vocabulary + seams. Header-only, no ROS/JSON. ---
add_library(mrs_core_pure INTERFACE)
target_include_directories(mrs_core_pure INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(mrs_core_pure INTERFACE cxx_std_17)

# --- mrs_core_msgs (CN-3): boundary string<->uint8 mapping table. Links mrs_msgs. ---
add_library(mrs_core_msgs
  src/reason_codes.cpp)
target_include_directories(mrs_core_msgs PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(mrs_core_msgs PUBLIC mrs_core_pure)
ament_target_dependencies(mrs_core_msgs PUBLIC mrs_msgs)

# --- startup_checker (🔴 STUB — see README.md and src/startup_checker_main.cpp). ---
add_executable(startup_checker src/startup_checker_main.cpp)
target_link_libraries(startup_checker PRIVATE mrs_core_msgs)

# --- tests (CTest; self-contained assert mains — module precedent, no gtest). ---
enable_testing()

add_executable(test_status test/test_status.cpp)
target_link_libraries(test_status PRIVATE mrs_core_pure)
add_test(NAME test_status COMMAND test_status)

add_executable(test_reason_codes test/test_reason_codes.cpp)
target_link_libraries(test_reason_codes PRIVATE mrs_core_msgs)
add_test(NAME test_reason_codes COMMAND test_reason_codes)

install(DIRECTORY include/ DESTINATION include)
install(TARGETS mrs_core_pure
  EXPORT export_mrs_core)
install(TARGETS mrs_core_msgs startup_checker
  EXPORT export_mrs_core
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION lib/${PROJECT_NAME}
  INCLUDES DESTINATION include)

ament_export_targets(export_mrs_core HAS_LIBRARY_TARGET)
ament_export_dependencies(mrs_msgs)

ament_package()
