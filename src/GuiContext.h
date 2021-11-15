#pragma once
#include "ProgramSelector.h"
#include "CameraManager.h"
#include "PlaybackState.h"
#include "InputController.h"
#include "graphics/Material.h"

struct GuiContext {
    ProgramSelector& programSelector;
    CameraManager& cameraManager;
    PlaybackState& playbackState;
    InputController& inputController;
    Material& globalMaterial;
};
