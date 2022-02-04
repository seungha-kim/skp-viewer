#pragma once

#include <optional>
#include "../../engine/scene/SceneManager.h"
#include "../PlaybackState.h"
#include "../input/InputController.h"
#include "../../engine/render/RenderMaterial.h"
#include "../../engine/render/SurfaceInfo.h"
#include "../../engine/render/Renderer.h"
#include "../../engine/runtime/RuntimeModel.h"

struct GuiContext {
    acon::SceneManager& sceneManager;
    PlaybackState& playbackState;
    InputController& inputController;
    const acon::SurfaceInfo& surfaceInfo;
    acon::RenderOptions& renderOptions;
    acon::RuntimeModel& runtimeModel;
    std::optional<acon::ObjectId>& selectedObjectIdOpt;
};
