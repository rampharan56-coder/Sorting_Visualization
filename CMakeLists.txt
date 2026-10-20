cmake_minimum_required(VERSION 3.10)
project(SortingVisualizer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_executable(sorting_visualizer ${SOURCES})
target_include_directories(sorting_visualizer PRIVATE "${CMAKE_SOURCE_DIR}/include")

if(MSVC)
    target_compile_options(sorting_visualizer PRIVATE /W4)
else()
    target_compile_options(sorting_visualizer PRIVATE -Wall -Wextra)
endif()

if(WIN32)
    target_link_libraries(sorting_visualizer PRIVATE)
else()
    find_package(Threads REQUIRED)
    target_link_libraries(sorting_visualizer PRIVATE Threads::Threads)
endif()
