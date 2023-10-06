cmake_minimum_required(VERSION 3.26)
project(MARCHING_CUBES)

set(CMAKE_CXX_STANDARD 17)

add_executable(MARCHING_CUBES main.cpp
        LIBRARIES/glad/glad.c
        LIBRARIES/stb/stb.cpp)
