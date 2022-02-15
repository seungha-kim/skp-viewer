#pragma once

#include "../../engine/render/RenderMaterial.h"
#include "../../engine/render/Renderer.h"
#include "../../engine/render/SurfaceInfo.h"
#include "../../engine/runtime/RuntimeModel.h"
#include "../../engine/scene/SceneManager.h"
#include "../PlaybackState.h"
#include "../input/InputController.h"
#include <optional>

struct GuiContext {
    acon::SceneManager& sceneManager;
    PlaybackState& playbackState;
    InputController& inputController;
    const acon::SurfaceInfo& surfaceInfo;
    acon::RenderOptions& renderOptions;
    acon::RuntimeModel& runtimeModel;
    acon::RenderModel& renderModel;
    std::optional<acon::ObjectId>& selectedObjectIdOpt;
    bool& hoveringGui;
};
