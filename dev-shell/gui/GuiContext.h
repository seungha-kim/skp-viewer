#pragma once

#include "../../engine/scene/SceneManager.h"
#include "../PlaybackState.h"
#include "../input/InputController.h"
#include "../../engine/render/RenderMaterial.h"
#include "../../engine/render/SurfaceInfo.h"
#include "../../engine/render/Renderer.h"

struct GuiContext {
    acon::SceneManager& sceneManager;
    PlaybackState& playbackState;
    InputController& inputController;
    const acon::SurfaceInfo& surfaceInfo;
    acon::RenderOptions& renderOptions;
};
