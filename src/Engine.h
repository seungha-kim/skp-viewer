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

    [[nodiscard]] const InputController& inputController() const;
    InputController& inputControllerMut();

    [[nodiscard]] const SurfaceInfo& surfaceInfo() const;
    SurfaceInfo& surfaceInfoMut();

    void setRandomGlobalDiffuse();

    void render(float playbackValue);
    void renderGui();
private:
    SceneManager m_sceneManager;
    InputController m_inputController;

    PlaybackState m_playbackState;
    SurfaceInfo m_surfaceInfo;

    ProgramSelector m_selector;

    Material m_globalMaterial;

#ifdef ENABLE_IMGUI
    Gui m_gui;
#endif

};
