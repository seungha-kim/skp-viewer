#pragma once

#include "export.h"
#include "scene/SceneManager.h"
#include "render/SurfaceInfo.h"
#include "render/RenderMaterial.h"
#include "render/Renderer.h"
#include "reader/AbstractReader.h"

namespace acon {

class DLL_EXPORT Engine {
public:
    explicit Engine(SurfaceInfo surfaceInfo, const AbstractReader& reader);
    ~Engine();

    [[nodiscard]] const SceneManager& sceneManager() const;
    SceneManager& sceneManagerMut();

    [[nodiscard]] const SurfaceInfo& surfaceInfo() const;

    Renderer& rendererMut();

    RuntimeModel& runtimeModelMut();

    CameraState& currentCameraStateMut();

    void render(float playbackValue, std::optional<ObjectId> selectedObjectIdOpt);
    void resize(const SurfaceInfo& surfaceInfo);

private:
    SceneManager m_sceneManager;
    SurfaceInfo m_surfaceInfo;
    std::unique_ptr<RuntimeModel> m_runtimeModel;
    std::unique_ptr<RenderModel> m_renderModel;
    std::unique_ptr<Renderer> m_renderer;

    bool m_sizeUpdated = false;

    void updateTextures();
};

}
