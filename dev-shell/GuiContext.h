#pragma once

#include "../src/scene/SceneManager.h"
#include "PlaybackState.h"
#include "InputController.h"
#include "../src/model/Material.h"
#include "../src/render/SurfaceInfo.h"
#include "../src/render/Renderer.h"

struct GuiContext {
    SceneManager& sceneManager;
    PlaybackState& playbackState;
    InputController& inputController;
    Material& globalMaterial;
    const SurfaceInfo& surfaceInfo;
    RenderOptions& renderOptions;
};
