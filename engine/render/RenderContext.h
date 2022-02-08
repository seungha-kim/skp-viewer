#pragma once

#include "RenderMaterial.h"
#include "../scene/Scene.h"
#include "SurfaceInfo.h"
#include <optional>
#include "RenderModel.h"
#include "../runtime/RuntimeModel.h"
namespace acon {

struct RenderContext {
    const Scene& scene;
    const float playbackValue;
    const SurfaceInfo& surfaceInfo;
    RenderModel& renderModel;
    const RuntimeModel& runtimeModel;
    const std::optional<ObjectId> selectedObjectIdOpt;
};

}
