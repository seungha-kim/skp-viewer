# Target - Renderer
add_library(engine
        STATIC # TODO: SHARED 로 하니까 윈도에서 에러 - "Windows fatal exception: access violation"
        render/checkError.cpp
        render/checkError.h
        render/Shader.cpp
        render/Shader.h
        lib.cpp
        scene/CameraState.h
        scene/SceneManager.cpp
        scene/SceneManager.h
        render/model/RenderMesh.cpp
        render/model/RenderMesh.h
        render/model/RenderVertex.h
        render/model/RenderMaterial.h
        scene/DirectionalLight.h
        scene/Scene.cpp scene/Scene.h
        render/SurfaceInfo.h
        render/Renderer.cpp
        render/Renderer.h
        render/SunlightPass.cpp
        render/SunlightPass.h
        render/MainPass.cpp
        render/MainPass.h
        render/RenderContext.h
        render/DepthTexture.cpp
        render/DepthTexture.h
        render/ColorTexture.cpp
        render/ColorTexture.h
        render/OffscreenRenderTarget.cpp
        render/OffscreenRenderTarget.h
        render/TextureRenderer.cpp
        render/TextureRenderer.h
        render/ColorBalancePass.cpp
        render/ColorBalancePass.h
        render/GaussianBlurPass.cpp
        render/GaussianBlurPass.h
        render/AdditiveBlendPass.cpp
        render/AdditiveBlendPass.h
        render/BrightFilterPass.cpp
        render/BrightFilterPass.h
        render/ToneMapPass.cpp
        render/ToneMapPass.h
        render/OutlinePass.cpp
        render/OutlinePass.h
        Engine.cpp
        Engine.h
        export.h
        reader/AbstractReader.h
        reader/AssimpReader.cpp
        reader/AssimpReader.h
        reader/SketchupReader.cpp
        reader/SketchupReader.h
        reader/sketchup-specialization.h
        render/model/RenderTexture.cpp
        render/model/RenderTexture.h
        runtime/RuntimeModel.h
        primitives.h
        runtime/RuntimeModel.cpp
        render/model/RenderModel.h
        render/model/RenderModel.cpp
        build.h
        build.cpp
        )

if (APPLE)
    find_library(SKETCHUP_LIBRARY
            NAMES SketchUpAPI
            HINTS ${CMAKE_CURRENT_LIST_DIR}/../extern/sketchup)
    if (NOT SKETCHUP_LIBRARY)
        message(FATAL_ERROR "SketchUpAPI not found")
    endif()
    add_library(sketchup-api INTERFACE IMPORTED)
    set_target_properties(sketchup-api PROPERTIES
            INTERFACE_LINK_LIBRARIES "${SKETCHUP_LIBRARY}")
endif()

if (WIN32)
    find_path(SKETCHUP_SDK_ROOT NAMES version.txt PATHS ${CMAKE_CURRENT_LIST_DIR}/../extern/sketchup)
    message(STATUS "SKETCHUP_SDK_ROOT: ${SKETCHUP_SDK_ROOT}")

    add_library(sketchup-api SHARED IMPORTED)
    set_property(TARGET sketchup-api PROPERTY IMPORTED_LOCATION ${SKETCHUP_SDK_ROOT}/binaries/sketchup/x64/SketchUpAPI.dll)
    set_property(TARGET sketchup-api PROPERTY IMPORTED_IMPLIB ${SKETCHUP_SDK_ROOT}/binaries/sketchup/x64/SketchUpAPI.lib)
    target_include_directories(sketchup-api INTERFACE ${SKETCHUP_SDK_ROOT}/headers)
endif()

message(SKETCHUP_LIBRARY ${SKETCHUP_LIBRARY})

target_link_libraries(engine
        PUBLIC
        assimp
        glad
        imgui
        sketchup-api
        )
