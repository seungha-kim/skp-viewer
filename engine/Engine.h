#pragma once

#include "export.h"
#include "scene/SceneManager.h"
#include "render/SurfaceInfo.h"
#include "render/RenderMaterial.h"
#include "render/Renderer.h"
#include "reader/AbstractReader.h"

namespace acon {

// NOTE: Engine 의 인터페이스는 Python binding 을 고려해서 설계해야 함.
class DLL_EXPORT Engine {
public:
    explicit Engine(const AbstractReader& reader);
    ~Engine();

    [[nodiscard]] const SceneManager& sceneManager() const;
    SceneManager& sceneManagerMut();

    [[nodiscard]] const SurfaceInfo& surfaceInfo() const;

    Renderer& rendererMut();

    RuntimeModel& runtimeModelMut();
    RenderModel& renderModelMut();

    CameraState& currentCameraStateMut();

    void prepareToRender(const SurfaceInfo& surfaceInfo);
    void render(float playbackValue);
    void resize(const SurfaceInfo& surfaceInfo);

private:
    SceneManager m_sceneManager{};
    SurfaceInfo m_surfaceInfo; // TODO: 여기서 제거
    std::unique_ptr<RuntimeModel> m_runtimeModel;
    std::unique_ptr<RenderModel> m_renderModel;
    std::unique_ptr<Renderer> m_renderer;

    bool m_sizeUpdated = false;
    bool m_preparedToRender = false;

    void updateTextures();
};

}
