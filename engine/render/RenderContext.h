#pragma once

#include "RenderMaterial.h"
#include "../scene/Scene.h"
#include "SurfaceInfo.h"
#include <optional>
#include "RenderModel.h"

namespace acon {

struct RenderContext {
    const Scene& scene;
    const float playbackValue;
    const SurfaceInfo& surfaceInfo;
    const RenderModel& renderModel;
    const std::optional<ObjectId> selectedObjectIdOpt;
};

}
