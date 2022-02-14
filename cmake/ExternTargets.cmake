# Extern - sketchup
if (APPLE)
    find_library(SKETCHUP_LIBRARY
            NAMES SketchUpAPI
            HINTS ${CMAKE_SOURCE_DIR}/extern/sketchup/macos)
    if (NOT SKETCHUP_LIBRARY)
        message(FATAL_ERROR "SketchUpAPI not found")
    endif()
    add_library(sketchup-api INTERFACE IMPORTED)
    set_target_properties(sketchup-api PROPERTIES
            INTERFACE_LINK_LIBRARIES "${SKETCHUP_LIBRARY}")
endif()

if (WIN32)
    find_path(SKETCHUP_SDK_ROOT NAMES version.txt PATHS ${CMAKE_SOURCE_DIR}/extern/sketchup/windows)

    add_library(sketchup-api SHARED IMPORTED)
    set(SKETCHUP_API_DLL ${SKETCHUP_SDK_ROOT}/binaries/sketchup/x64/SketchUpAPI.dll)
    set(SKETCHUP_API_COMMON_DLL ${SKETCHUP_SDK_ROOT}/binaries/sketchup/x64/SketchUpCommonPreferences.dll)
    set(SKETCHUP_API_IMPLIB ${SKETCHUP_SDK_ROOT}/binaries/sketchup/x64/SketchUpAPI.lib)
    set_property(TARGET sketchup-api PROPERTY IMPORTED_LOCATION ${SKETCHUP_API_DLL})
    set_property(TARGET sketchup-api PROPERTY IMPORTED_IMPLIB ${SKETCHUP_API_IMPLIB})
    
    list(APPEND SKETCHUP_DLL_LIST ${SKETCHUP_API_DLL})
    list(APPEND SKETCHUP_DLL_LIST ${SKETCHUP_API_COMMON_DLL})

    target_include_directories(sketchup-api INTERFACE ${SKETCHUP_SDK_ROOT}/headers)
endif()

# Extern - nativefiledialog-extended
add_subdirectory(extern/nfd)

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
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE PYTHON_INCLUDE_DIRS)
execute_process(COMMAND python3 scripts/find_python.py --lib
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE PYTHON_LIBRARY_DIR)
execute_process(COMMAND python3 scripts/find_python.py --name
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE PYTHON_LIBRARY_NAME)
find_library(PYTHON_LIBRARIES
        NAMES ${PYTHON_LIBRARY_NAME}
        PATHS ${PYTHON_LIBRARY_DIR})
add_library(python INTERFACE)
target_link_libraries(python INTERFACE ${PYTHON_LIBRARIES})
target_include_directories(python INTERFACE ${PYTHON_INCLUDE_DIRS})
