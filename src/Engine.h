#pragma once

#include "export.h"
#include "PlaybackState.h"
#include "SceneManager.h"
#include "../dev-shell/InputController.h"
#include "SurfaceInfo.h"
#include "graphics/Material.h"
#include "../dev-shell/Gui.h"
#include "render/MonkeyProgram.h"

class DLL_EXPORT Engine {
public:
    explicit Engine(SurfaceInfo surfaceInfo);

    [[nodiscard]] const PlaybackState& playbackState() const;

    [[nodiscard]] const SceneManager& sceneManager() const;
    SceneManager& sceneManagerMut();

    [[nodiscard]] const SurfaceInfo& surfaceInfo() const;

    void setRandomGlobalDiffuse();

    void render(float playbackValue);
    void renderGui();
    void resize(const SurfaceInfo& surfaceInfo);

    void onMouseMove(float mousePosX, float mousePosY);
    void onMouseWheel(float mousePosX, float mousePosY, float wheelOffsetX, float wheelOffsetY);
    void onKeyboardStateChange(const KeyCommandSet& keyCommandSet);
    void handleInput();

    void updateTime(float time);

    [[nodiscard]] bool shouldClose() const;
    [[nodiscard]] bool showMouseCursor() const;

    [[nodiscard]] bool shouldContinuouslyRender() const;

private:
    SceneManager m_sceneManager;
    InputController m_inputController;

    PlaybackState m_playbackState;
    SurfaceInfo m_surfaceInfo;
    MonkeyProgram m_renderer;

    Material m_globalMaterial;

    // Input
    float m_mousePosX;
    float m_mousePosY;
    std::optional<MouseEvent> m_mouseEvent;
    KeyCommandSet m_keyCommandSet;
    KeyCommandSet m_prevKeyCommandSet;
    bool m_shouldClose = false;
    bool m_showMouseCursor = true;

    bool m_sizeUpdated = false;

    void updateTextures();

#ifdef ENABLE_IMGUI
    Gui m_gui;
#endif

};
