#pragma once

#include "export.h"
#include "scene/SceneManager.h"
#include "render/SurfaceInfo.h"
#include "render/model/RenderMaterial.h"
#include "render/Renderer.h"
#include "reader/AbstractReader.h"

namespace acon {

class DLL_EXPORT Engine {
public:
    explicit Engine(SurfaceInfo surfaceInfo);

    [[nodiscard]] const SceneManager& sceneManager() const;
    SceneManager& sceneManagerMut();

    [[nodiscard]] const SurfaceInfo& surfaceInfo() const;
    RenderMaterial& globalMaterialMut();

    Renderer& rendererMut();

    CameraState& currentCameraStateMut();

    void setRandomGlobalDiffuse();

    void render(float playbackValue);
    void resize(const SurfaceInfo& surfaceInfo);

private:
    SceneManager m_sceneManager;
    std::unique_ptr<AbstractReader> m_model; // TODO: persistent model 로 바꾸기
    SurfaceInfo m_surfaceInfo;
    Renderer m_renderer;
    RenderMaterial m_globalMaterial;

    bool m_sizeUpdated = false;

    void updateTextures();
};

}
