cmake_minimum_required(VERSION 3.20)
set(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "" FORCE)
project(skp-viewer)

set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

add_compile_definitions(GL_SILENCE_DEPRECATION)

include(ExternTargets)

# Settings after building dependencies
add_compile_definitions(GLFW_INCLUDE_NONE)

add_subdirectory(engine)
add_subdirectory(dev-shell)
add_subdirectory(bindings)

# Resources
set(RESOURCES_SOURCE "${CMAKE_CURRENT_LIST_DIR}/resources")
set(RESOURCES_DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/resources")
