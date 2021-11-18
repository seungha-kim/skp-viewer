#pragma once
#include "ProgramSelector.h"
#include "SceneManager.h"
#include "PlaybackState.h"
#include "InputController.h"
#include "graphics/Material.h"
#include "WindowDimension.h"

struct GuiContext {
    ProgramSelector& programSelector;
    SceneManager& sceneManager;
    PlaybackState& playbackState;
    InputController& inputController;
    Material& globalMaterial;
    const WindowDimension& windowDimension;
};
