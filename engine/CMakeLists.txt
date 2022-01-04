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
        scene/DirectionalLight.cpp
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
        )

target_link_libraries(engine
        PUBLIC
        assimp glad imgui)
