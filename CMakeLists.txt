cmake_minimum_required(VERSION 3.20)
project(learn_opengl)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2)

add_executable(learn_opengl main.cpp)
include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(learn_opengl ${SDL2_LIBRARIES})