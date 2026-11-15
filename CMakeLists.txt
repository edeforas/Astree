cmake_minimum_required(VERSION 3.20.0)
project(astree LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ASTREE_BUILD_GUI "Build the Qt desktop application" ON)

add_subdirectory(src)
add_subdirectory(tests)

if(ASTREE_BUILD_GUI)
    add_subdirectory(gui)
endif()
