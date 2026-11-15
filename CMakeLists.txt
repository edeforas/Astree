cmake_minimum_required(VERSION 3.20.0)
project(astree LANGUAGES CXX)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ASTREE_BUILD_IMGUI "Build the Dear ImGui desktop application" ON)

add_subdirectory(core)
add_subdirectory(tests)

if(ASTREE_BUILD_IMGUI)
    add_subdirectory(gui)
endif()
