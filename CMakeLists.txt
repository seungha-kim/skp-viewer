cmake_minimum_required(VERSION 3.20)
set(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "" FORCE)
project(learn_opengl)

set(CMAKE_CXX_STANDARD 20)
add_compile_definitions(GL_SILENCE_DEPRECATION)

# Extern - pybind11
add_subdirectory(extern/pybind11)

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
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
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

# Extern - python
execute_process(COMMAND python3 scripts/find_python.py --include
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        OUTPUT_VARIABLE PYTHON_INCLUDE_DIRS)
execute_process(COMMAND python3 scripts/find_python.py --lib
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        OUTPUT_VARIABLE PYTHON_LIBRARY_DIR)
execute_process(COMMAND python3 scripts/find_python.py --name
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        OUTPUT_VARIABLE PYTHON_LIBRARY_NAME)
find_library(PYTHON_LIBRARIES
        NAMES ${PYTHON_LIBRARY_NAME}
        PATHS ${PYTHON_LIBRARY_DIR})
add_library(python INTERFACE)
target_link_libraries(python INTERFACE ${PYTHON_LIBRARIES})
target_include_directories(python INTERFACE ${PYTHON_INCLUDE_DIRS})

# Settings after building dependencies
add_compile_definitions(GLFW_INCLUDE_NONE)

# Target - Renderer
add_library(renderer
        STATIC # TODO: SHARED 로 하니까 윈도에서 에러 - "Windows fatal exception: access violation"
        src/checkError.cpp
        src/checkError.h
        src/Shader.cpp
        src/Shader.h
        src/lib.cpp
        src/CameraState.h
        src/PlaybackState.h
        src/SceneManager.cpp
        src/SceneManager.h
        src/ContinuousRenderSession.h
        src/graphics/Mesh.cpp
        src/graphics/Mesh.h
        src/graphics/Vertex.cpp
        src/graphics/Vertex.h
        src/graphics/Material.h
        src/graphics/DirectionalLight.cpp
        src/graphics/DirectionalLight.h
        src/Scene.cpp src/Scene.h
        src/SurfaceInfo.h
        src/Program.h
        src/render/Renderer.cpp
        src/render/Renderer.h
        src/render/SunlightPass.cpp
        src/render/SunlightPass.h
        src/render/MainPass.cpp
        src/render/MainPass.h
        src/RenderContext.h
        src/DepthTexture.cpp
        src/DepthTexture.h
        src/ColorTexture.cpp
        src/ColorTexture.h
        src/OffscreenRenderTarget.cpp
        src/OffscreenRenderTarget.h
        src/TextureRenderer.cpp
        src/TextureRenderer.h
        src/render/ColorBalancePass.cpp
        src/render/ColorBalancePass.h
        src/render/GaussianBlurPass.cpp
        src/render/GaussianBlurPass.h
        src/render/AdditiveBlendPass.cpp
        src/render/AdditiveBlendPass.h
        src/render/BrightFilterPass.cpp
        src/render/BrightFilterPass.h
        src/render/ToneMapPass.cpp
        src/render/ToneMapPass.h
        src/render/OutlinePass.cpp
        src/render/OutlinePass.h
        src/Engine.cpp
        src/Engine.h
        src/export.h
        )

target_link_libraries(renderer
        PUBLIC
        assimp glad imgui)

add_subdirectory(dev-shell)

# Target - Binding
pybind11_add_module(binding_test
        src/bindings.cpp)
target_link_libraries(binding_test
        PRIVATE
        renderer
        pybind11_headers
        python)

# Resources
set(RESOURCES_SOURCE "${CMAKE_CURRENT_LIST_DIR}/resources")
set(RESOURCES_DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/resources")

# Assimp Test
add_executable(assimp_test src/assimp_test.cpp)
target_link_libraries(assimp_test assimp)
