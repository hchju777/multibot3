cmake_minimum_required(VERSION 3.28)
project(mrs_msg_convert)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()
add_compile_options(-Wall -Wextra -Wpedantic -Werror)

find_package(ament_cmake REQUIRED)
find_package(mrs_core_types REQUIRED)
find_package(mrs_interfaces REQUIRED)
find_package(mrs_map_registry REQUIRED)
find_package(mrs_planning REQUIRED)
find_package(mrs_depgraph REQUIRED)
find_package(mrs_reorder_btpg REQUIRED)
find_package(mrs_reorder_milp REQUIRED)
find_package(mrs_judge REQUIRED)
find_package(mrs_task_alloc REQUIRED)
find_package(mrs_ladder REQUIRED)
find_package(mrs_local_traj REQUIRED)
find_package(mrs_sim_abstraction REQUIRED)
find_package(mrs_metrics REQUIRED)

file(GLOB_RECURSE MRS_MSG_CONVERT_SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_library(${PROJECT_NAME} ${MRS_MSG_CONVERT_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include>")
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
ament_target_dependencies(${PROJECT_NAME} PUBLIC mrs_core_types mrs_interfaces mrs_map_registry mrs_planning mrs_depgraph mrs_reorder_btpg mrs_reorder_milp mrs_judge mrs_task_alloc mrs_ladder mrs_local_traj mrs_sim_abstraction mrs_metrics)

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
ament_export_dependencies(mrs_core_types mrs_interfaces mrs_map_registry mrs_planning mrs_depgraph mrs_reorder_btpg mrs_reorder_milp mrs_judge mrs_task_alloc mrs_ladder mrs_local_traj mrs_sim_abstraction mrs_metrics)


if(BUILD_TESTING)
  # gtest 골격은 Phase 5(test-author)가 내용과 함께 생성한다 (design-decision v5 build_policy).
  # 빈 테스트 파일을 두지 않는다 — 자리만 남긴다.
endif()

ament_package()
