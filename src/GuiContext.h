#pragma once
#include "ProgramSelector.h"
#include "CameraManager.h"
#include "PlaybackState.h"
#include "InputController.h"

struct GuiContext {
    ProgramSelector& programSelector;
    CameraManager& cameraManager;
    PlaybackState& playbackState;
    InputController& inputController;
};
