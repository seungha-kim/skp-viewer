#pragma once

#include "export.h"
#include "scene/SceneManager.h"
#include "render/SurfaceInfo.h"
#include "model/Material.h"
#include "render/Renderer.h"

class DLL_EXPORT Engine {
public:
    explicit Engine(SurfaceInfo surfaceInfo);

    [[nodiscard]] const SceneManager& sceneManager() const;
    SceneManager& sceneManagerMut();

    [[nodiscard]] const SurfaceInfo& surfaceInfo() const;
    Material& globalMaterialMut();

    Renderer& rendererMut();

    void setRandomGlobalDiffuse();

    void render(float playbackValue);
    void resize(const SurfaceInfo& surfaceInfo);

private:
    SceneManager m_sceneManager;

    SurfaceInfo m_surfaceInfo;
    Renderer m_renderer;
    Material m_globalMaterial;

    bool m_sizeUpdated = false;

    void updateTextures();
};
