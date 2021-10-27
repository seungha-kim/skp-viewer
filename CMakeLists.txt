cmake_minimum_required(VERSION 3.20)
project(learn_opengl)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)

# Extern - glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(extern/glfw)
include_directories(extern/glfw/include)

# Extern - assimp
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(extern/assimp)
include_directories(extern/assimp/include)


add_executable(learn_opengl main.cpp)
target_link_libraries(learn_opengl OpenGL::GL glfw assimp)
