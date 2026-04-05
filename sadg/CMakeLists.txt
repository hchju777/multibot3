cmake_minimum_required(VERSION 3.16)
project(sadg CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)

add_compile_options(-Wall -Wextra -Wpedantic)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../mapf
                 ${CMAKE_CURRENT_BINARY_DIR}/mapf_build)

set(SADG_SOURCES
    src/sadg_graph.cpp
    src/sadg_builder.cpp
    src/sadg_scheduler.cpp
    src/sadg_validator.cpp
)

add_library(sadg_lib STATIC ${SADG_SOURCES})
target_include_directories(sadg_lib
    PUBLIC
        include
        ${CMAKE_CURRENT_SOURCE_DIR}/../mapf/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../mapf/test
)
target_link_libraries(sadg_lib PUBLIC mapf_lib)

if(BUILD_TESTING)
    add_executable(sadg_unit_tests test/sadg_unit_tests.cpp)
    target_include_directories(sadg_unit_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../mapf/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../mapf/test
    )
    target_link_libraries(sadg_unit_tests PRIVATE sadg_lib)
    add_test(NAME sadg_unit_tests COMMAND sadg_unit_tests)
endif()
