cmake_minimum_required(VERSION 3.26)
project(MARCHING_CUBES)

set(CMAKE_CXX_STANDARD 17)

add_executable(MARCHING_CUBES main.cpp
        LIBRARIES/glad/glad.c
        LIBRARIES/stb/stb.cpp)

target_include_directories(MARCHING_CUBES PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

find_package(glfw3 REQUIRED)
target_link_libraries(MARCHING_CUBES PRIVATE glfw)

find_package(OpenGL REQUIRED)
target_link_libraries(MARCHING_CUBES PRIVATE OpenGL::GL)

