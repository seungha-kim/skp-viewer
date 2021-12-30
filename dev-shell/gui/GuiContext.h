#pragma once

#include "../../engine/scene/SceneManager.h"
#include "../PlaybackState.h"
#include "../input/InputController.h"
#include "../../engine/model/Material.h"
#include "../../engine/render/SurfaceInfo.h"
#include "../../engine/render/Renderer.h"

struct GuiContext {
    SceneManager& sceneManager;
    PlaybackState& playbackState;
    InputController& inputController;
    Material& globalMaterial;
    const SurfaceInfo& surfaceInfo;
    RenderOptions& renderOptions;
};
