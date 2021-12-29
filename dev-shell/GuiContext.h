#pragma once
#include "../src/ProgramSelector.h"
#include "../src/SceneManager.h"
#include "../src/PlaybackState.h"
#include "InputController.h"
#include "../src/graphics/Material.h"
#include "../src/SurfaceInfo.h"

struct GuiContext {
    ProgramSelector& programSelector;
    SceneManager& sceneManager;
    PlaybackState& playbackState;
    InputController& inputController;
    Material& globalMaterial;
    const SurfaceInfo& surfaceInfo;
};
