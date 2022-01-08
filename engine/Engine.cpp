#include "Engine.h"
#include "render/checkError.h"
#include <glad/glad.h>
#include <glm/gtc/random.hpp>
#include "reader/AssimpReader.h"
#include "reader/SketchupReader.h"

namespace acon {

Engine::Engine(SurfaceInfo surfaceInfo, const AbstractReader& model)
    : m_surfaceInfo(surfaceInfo)
    , m_globalMaterial{
        .ambient = glm::vec3(0.1f, 0.1f, 0.1f),
        .diffuse = glm::vec3(0.3f, 0.6f, 0.2f),
        .specular = glm::vec3(1.0f, 1.0f, 1.0f),
        .shininess = 32.0f,
    }
    , m_sceneManager(surfaceInfo)
    , m_renderer(surfaceInfo, model) {
    glEnable(GL_DEPTH_TEST);
}

const SceneManager &Engine::sceneManager() const {
    return m_sceneManager;
}

SceneManager &Engine::sceneManagerMut() {
    return m_sceneManager;
}

const SurfaceInfo &Engine::surfaceInfo() const {
    return m_surfaceInfo;
}

void Engine::render(float playbackValue) {
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("WARN - NOT COMPLETE\n");
        return;
    }
    updateTextures();
//    printf("RENDER %f\n", playbackValue);
    RenderContext renderCtx {
            .scene = sceneManager().activeScene(),
            .playbackValue = playbackValue,
            .globalMaterial = m_globalMaterial,
            .surfaceInfo = m_surfaceInfo,
    };
    m_renderer.render(renderCtx);
}

void Engine::setRandomGlobalDiffuse() {
    m_globalMaterial.diffuse = glm::linearRand(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
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
    m_renderer.resizeResources(m_surfaceInfo);
}

RenderMaterial& Engine::globalMaterialMut() {
    return m_globalMaterial;
}

Renderer& Engine::rendererMut() {
    return m_renderer;
}

CameraState& Engine::currentCameraStateMut() {
    return m_sceneManager.activeSceneMut().cameraStateMut();
}

}
