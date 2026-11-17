cmake_minimum_required(VERSION 3.16)
project(SortingVisualizer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Disable AUTOMOC / AUTOUIC / AUTORCC:
# All UI widgets and lambda signal connections are written in standard C++17
# without .ui files or Q_OBJECT macros, avoiding uic.exe / moc.exe DLL issues in Git Bash.
set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC OFF)
set(CMAKE_AUTOUIC OFF)

# Find Qt6 or Qt5 Widgets
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB HEADERS "${CMAKE_SOURCE_DIR}/include/*.h")

add_executable(sorting_visualizer ${SOURCES} ${HEADERS})
target_include_directories(sorting_visualizer PRIVATE "${CMAKE_SOURCE_DIR}/include")

if(MSVC)
    target_compile_options(sorting_visualizer PRIVATE /W4)
else()
    target_compile_options(sorting_visualizer PRIVATE -Wall -Wextra)
endif()

target_link_libraries(sorting_visualizer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

if(NOT WIN32)
    find_package(Threads REQUIRED)
    target_link_libraries(sorting_visualizer PRIVATE Threads::Threads)
endif()
