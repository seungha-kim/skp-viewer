#pragma once
#include "ProgramSelector.h"
#include "SceneManager.h"
#include "PlaybackState.h"
#include "InputController.h"
#include "graphics/Material.h"
#include "SurfaceInfo.h"

struct GuiContext {
    ProgramSelector& programSelector;
    SceneManager& sceneManager;
    PlaybackState& playbackState;
    InputController& inputController;
    Material& globalMaterial;
    const SurfaceInfo& surfaceInfo;
};
