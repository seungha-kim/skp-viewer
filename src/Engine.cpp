#include "Engine.h"
#include "checkError.h"
#include <glad/glad.h>
#include <glm/gtc/random.hpp>

Engine::Engine(SurfaceInfo surfaceInfo)
    : m_surfaceInfo(surfaceInfo)
#ifdef ENABLE_IMGUI
    , m_gui()
#endif
    , m_globalMaterial{
        .ambient = glm::vec3(0.1f, 0.1f, 0.1f),
        .diffuse = glm::vec3(0.3f, 0.6f, 0.2f),
        .specular = glm::vec3(1.0f, 1.0f, 1.0f),
        .shininess = 32.0f,
    }
    , m_selector(surfaceInfo) {
    glEnable(GL_DEPTH_TEST);
}

const PlaybackState &Engine::playbackState() const {
    return m_playbackState;
}

PlaybackState &Engine::playbackStateMut() {
    return m_playbackState;
}

const SceneManager &Engine::sceneManager() const {
    return m_sceneManager;
}

SceneManager &Engine::sceneManagerMut() {
    return m_sceneManager;
}

const InputController &Engine::inputController() const {
    return m_inputController;
}

InputController &Engine::inputControllerMut() {
    return m_inputController;
}

const SurfaceInfo &Engine::surfaceInfo() const {
    return m_surfaceInfo;
}

SurfaceInfo &Engine::surfaceInfoMut() {
    return m_surfaceInfo;
}

void Engine::render(float playbackValue) {
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("WARN - NOT COMPLETE\n");
        return;
    }
//    printf("RENDER %f\n", playbackValue);
    RenderContext renderCtx {
            .scene = sceneManager().activeScene(),
            .playbackValue = playbackValue,
            .globalMaterial = m_globalMaterial,
            .surfaceInfo = m_surfaceInfo,
    };
    m_selector.currentProgram().render(renderCtx);
}

void Engine::renderGui() {
    GuiContext guiCtx {
            .programSelector = m_selector,
            .sceneManager = sceneManagerMut(),
            .playbackState = playbackStateMut(),
            .inputController = inputControllerMut(),
            .globalMaterial = m_globalMaterial,
            .surfaceInfo = surfaceInfo(),
    };
#ifdef ENABLE_IMGUI
    m_selector.currentProgram().processGui(guiCtx);
    m_gui.process(guiCtx);
#endif
}

void Engine::setRandomGlobalDiffuse() {
    m_globalMaterial.diffuse = glm::linearRand(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
}
