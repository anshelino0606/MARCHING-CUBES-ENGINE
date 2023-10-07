cmake_minimum_required(VERSION 3.26)
project(MARCHING_CUBES)

set(CMAKE_CXX_STANDARD 17)

add_executable(MARCHING_CUBES SCENE/main.cpp
        LIBRARIES/glad/glad.c
        LIBRARIES/stb/stb.cpp
        ENGINE/Graphics/Shader/Shader.cpp
        ENGINE/Graphics/Texture/Texture.cpp
        ENGINE/Graphics/Cubemap/Cubemap.cpp
        ENGINE/Graphics/Material/Material.cpp
        ENGINE/Algorithms/States/States.cpp
        ENGINE/Graphics/Mesh/Mesh.cpp
        ENGINE/Graphics/Light/Light.cpp
        ENGINE/Algorithms/Octree/Octree.cpp
        ENGINE/Algorithms/Bounds/Bounds.cpp
        ENGINE/Graphics/Model/Model.cpp
        ENGINE/Physics/RigidBody/RigidBody.cpp)

target_include_directories(MARCHING_CUBES PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

find_package(glfw3 REQUIRED)
target_link_libraries(MARCHING_CUBES PRIVATE glfw)

find_package(OpenGL REQUIRED)
target_link_libraries(MARCHING_CUBES PRIVATE OpenGL::GL)

include_directories(/opt/homebrew/Cellar/glm/0.9.9.8)

set(ASSIMP_ROOT "/opt/homebrew/Cellar/assimp/5.2.5" CACHE PATH "Assimp root directory")
find_package(assimp REQUIRED HINTS ${ASSIMP_ROOT})
include_directories(${ASSIMP_INCLUDE_DIRS})
target_link_libraries(MARCHING_CUBES PRIVATE ${ASSIMP_LIBRARIES})

include_directories(libraries)