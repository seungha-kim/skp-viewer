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
    , m_sceneManager(surfaceInfo)
    , m_renderer(surfaceInfo) {
    glEnable(GL_DEPTH_TEST);
}

const PlaybackState &Engine::playbackState() const {
    return m_playbackState;
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

void Engine::renderGui() {
    GuiContext guiCtx {
            .sceneManager = sceneManagerMut(),
            .playbackState = m_playbackState,
            .inputController = m_inputController,
            .globalMaterial = m_globalMaterial,
            .surfaceInfo = surfaceInfo(),
    };
#ifdef ENABLE_IMGUI
    m_renderer.processGui(guiCtx);
    m_gui.process(guiCtx);
#endif
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

void Engine::onMouseMove(float mousePosX, float mousePosY) {
    m_mouseEvent = MouseMoveEvent {.x = mousePosX, .y = (float)mousePosY};
    m_mousePosX = mousePosX;
    m_mousePosY = mousePosY;
}

void Engine::onMouseWheel(float mousePosX, float mousePosY, float wheelOffsetX, float wheelOffsetY) {
    m_mouseEvent = MouseScrollEvent {.offsetX = (float)wheelOffsetX, .offsetY = (float)wheelOffsetY};
    m_mousePosX = mousePosX;
    m_mousePosY = mousePosY;
}

void Engine::onKeyboardStateChange(const KeyCommandSet& keyCommandSet) {
    m_keyCommandSet = keyCommandSet;
}

void Engine::handleInput() {
    InputContext ctx {
            .cameraManager = m_sceneManager,
            .playbackState = m_playbackState,
            .mouseEvent = m_mouseEvent,
            .keyCommandSet = m_keyCommandSet,
            .prevKeyCommandSet = m_prevKeyCommandSet,
            .shouldClose = m_shouldClose,
            .showMouseCursor = m_showMouseCursor,
            .mousePosX = m_mousePosX,
            .mousePosY = m_mousePosY,
    };
    m_inputController.handleKeyboardInput(ctx);
    m_inputController.handleMouseInput(ctx);

    m_prevKeyCommandSet = m_keyCommandSet;
}

bool Engine::shouldClose() const {
    return m_shouldClose;
}

bool Engine::showMouseCursor() const {
    return m_showMouseCursor;
}

void Engine::updateTime(float time) {
    if (m_playbackState.continuousRenderSession.has_value()) {
        m_playbackState.continuousRenderSession->updateTime(time);
    }
}

bool Engine::shouldContinuouslyRender() const {
    return m_playbackState.continuousRenderSession.has_value();
}
