#include "Engine.h"
#include "render/checkError.h"
#include <glad/glad.h>
#include <glm/gtc/random.hpp>
#include "reader/AssimpReader.h"
#include "reader/SketchupReader.h"
#include "render/RenderModel.h"
#include "runtime/RuntimeModel.h"

namespace acon {

Engine::Engine(SurfaceInfo surfaceInfo, const AbstractReader& reader)
    : m_surfaceInfo(surfaceInfo)
    , m_sceneManager(surfaceInfo) {
    auto [runtimeModel, renderModel] = buildModel(reader);
    m_runtimeModel = std::move(runtimeModel);
    m_renderModel = std::move(renderModel);
    m_renderer = std::make_unique<Renderer>(surfaceInfo);
    glEnable(GL_DEPTH_TEST);
}

Engine::~Engine() = default;

const SceneManager &Engine::sceneManager() const {
    return m_sceneManager;
}

SceneManager &Engine::sceneManagerMut() {
    return m_sceneManager;
}

const SurfaceInfo &Engine::surfaceInfo() const {
    return m_surfaceInfo;
}

void Engine::render(float playbackValue, std::optional<ObjectId> selectedObjectIdOpt) {
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("WARN - NOT COMPLETE\n");
        return;
    }
    updateTextures();
//    printf("RENDER %f\n", playbackValue);
    RenderContext renderCtx {
            .scene = sceneManager().activeScene(),
            .playbackValue = playbackValue,
            .surfaceInfo = m_surfaceInfo,
            .renderModel = *m_renderModel,
            .runtimeModel = *m_runtimeModel,
            .selectedObjectIdOpt = selectedObjectIdOpt,
    };
    m_renderer->render(renderCtx);
}

void Engine::resize(const SurfaceInfo &surfaceInfo) {
    m_surfaceInfo = surfaceInfo;
    m_sizeUpdated = true;
}

void Engine::updateTextures() {
    if (!m_sizeUpdated) return;
    m_sizeUpdated = false;

    auto& cam = m_sceneManager.activeSceneMut().cameraStateMut();
    cam.aspectWidth = (float)m_surfaceInfo.logicalWidth;
    cam.aspectHeight = (float)m_surfaceInfo.logicalHeight;
    m_renderer->resizeResources(m_surfaceInfo);
}

Renderer& Engine::rendererMut() {
    return *m_renderer;
}

RuntimeModel &Engine::runtimeModelMut() {
    return *m_runtimeModel;
}

RenderModel &Engine::renderModelMut() {
    return *m_renderModel;
}

CameraState& Engine::currentCameraStateMut() {
    return m_sceneManager.activeSceneMut().cameraStateMut();
}

}
