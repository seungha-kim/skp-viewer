#pragma once

#include "export.h"
#include "PlaybackState.h"
#include "SceneManager.h"
#include "InputController.h"
#include "SurfaceInfo.h"
#include "ProgramSelector.h"
#include "graphics/Material.h"
#include "Gui.h"

class DLL_EXPORT Engine {
public:
    explicit Engine(SurfaceInfo surfaceInfo);

    [[nodiscard]] const PlaybackState& playbackState() const;
    PlaybackState& playbackStateMut();

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

    [[nodiscard]] bool shouldClose() const;
    [[nodiscard]] bool showMouseCursor() const;

private:
    SceneManager m_sceneManager;
    InputController m_inputController;

    PlaybackState m_playbackState;
    SurfaceInfo m_surfaceInfo;

    ProgramSelector m_selector;

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
