#pragma once

#include "../src/SceneManager.h"
#include "../src/PlaybackState.h"
#include "InputController.h"
#include "../src/graphics/Material.h"
#include "../src/SurfaceInfo.h"
#include "../src/render/Renderer.h"

struct GuiContext {
    SceneManager& sceneManager;
    PlaybackState& playbackState;
    InputController& inputController;
    Material& globalMaterial;
    const SurfaceInfo& surfaceInfo;
    RenderOptions& renderOptions;
};
