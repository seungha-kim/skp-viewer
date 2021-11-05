cmake_minimum_required(VERSION 3.20)
project(learn_opengl)

set(CMAKE_CXX_STANDARD 17)
add_compile_definitions(GL_SILENCE_DEPRECATION)

# Extern - OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
message("OPENGL_INCLUDE_DIR ${OPENGL_INCLUDE_DIR}")

# Extern - glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(extern/glfw)
include_directories(extern/glfw/include)

# Extern - assimp
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)
add_subdirectory(extern/assimp)
include_directories(extern/assimp/include)

# Extern - glm
set(GLM_DIR extern/glm/cmake/glm)
find_package(GLM REQUIRED)
include_directories(${GLM_INCLUDE_DIRS})

# Extern - glad
include_directories(extern/glad/include)
add_library(glad extern/glad/src/glad.c)

# Extern - stb
include_directories(extern/stb)

# Extern - imgui
add_library(imgui
        extern/imgui/imgui.cpp
        extern/imgui/imgui_demo.cpp
        extern/imgui/imgui_draw.cpp
        extern/imgui/imgui_tables.cpp
        extern/imgui/imgui_widgets.cpp
        extern/imgui/backends/imgui_impl_glfw.cpp
        extern/imgui/backends/imgui_impl_opengl3.cpp)
target_include_directories(imgui
        PUBLIC
        extern/imgui
        extern/imgui/backends)

# Targets
add_executable(learn_opengl main.cpp Shader.cpp Shader.h lib.cpp Gui.cpp Gui.h CameraOption.h RenderOption.h)
target_link_libraries(learn_opengl OpenGL::GL glfw assimp glad imgui)
target_compile_definitions(learn_opengl PUBLIC GLFW_INCLUDE_NONE)

# Resources
set(RESOURCES_SOURCE "${CMAKE_CURRENT_LIST_DIR}/resources")
set(RESOURCES_DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/resources")
file(CREATE_LINK ${RESOURCES_SOURCE} ${RESOURCES_DESTINATION} SYMBOLIC)