cmake_minimum_required(VERSION 3.16)
project(mapf CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)

# ── 경고 플래그 ──────────────────────────────────────────────
add_compile_options(-Wall -Wextra -Wpedantic)

# ── yaml-cpp ─────────────────────────────────────────────────
find_package(yaml-cpp REQUIRED)
find_package(spdlog REQUIRED)

# ── 소스 목록 ────────────────────────────────────────────────
set(MAPF_SOURCES
    src/graph.cpp
    src/dist_table.cpp
    src/collision.cpp
    src/pibt.cpp
    src/stop_condition.cpp
    src/solution.cpp
    src/planner.cpp
    src/config_loader.cpp
    src/graph_preprocessor.cpp
    src/params.cpp
    src/logger.cpp
    src/solution_validation.cpp
    src/map_loader.cpp
    src/prepared_map.cpp
    src/goal_distance_cache.cpp
    src/scenario_loader.cpp
    src/lifelong_planner.cpp
)

# ── 라이브러리 (다른 모듈 / ROS2 에서 링크용) ───────────────
add_library(mapf_lib STATIC ${MAPF_SOURCES})
target_include_directories(mapf_lib PUBLIC include)
target_link_libraries(mapf_lib PUBLIC yaml-cpp spdlog::spdlog)

# ── 간단한 smoke-test 실행파일 ───────────────────────────────
add_executable(mapf_test test/smoke_test.cpp)
target_link_libraries(mapf_test PRIVATE mapf_lib)

add_executable(mapf_lifelong_test test/lifelong_smoke_test.cpp)
target_link_libraries(mapf_lifelong_test PRIVATE mapf_lib)

# ── unit / regression tests ──────────────────────────────────
if(BUILD_TESTING)
    add_executable(mapf_unit_tests test/unit_tests.cpp)
    target_link_libraries(mapf_unit_tests PRIVATE mapf_lib)
    target_compile_definitions(mapf_unit_tests PRIVATE MAPF_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
    add_test(NAME mapf_unit_tests COMMAND mapf_unit_tests)

    add_executable(mapf_regression_tests test/regression_tests.cpp)
    target_link_libraries(mapf_regression_tests PRIVATE mapf_lib)
    target_compile_definitions(mapf_regression_tests PRIVATE MAPF_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
    add_test(NAME mapf_regression_tests COMMAND mapf_regression_tests)
endif()
