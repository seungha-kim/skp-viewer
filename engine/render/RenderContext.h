#pragma once

#include "../runtime/RuntimeModel.h"
#include "../scene/Scene.h"
#include "RenderMaterial.h"
#include "RenderModel.h"
#include "SurfaceInfo.h"
#include <optional>
namespace acon {

class RenderQuery {
public:
    virtual ~RenderQuery() = default;
    [[nodiscard]] virtual bool isSelfOrParentSelected(ObjectId objectId) const = 0;
};

struct RenderContext {
    const Scene& scene;
    const float playbackValue;
    const SurfaceInfo& surfaceInfo;
    RenderModel& renderModel;
    const RenderQuery& query;
};

}
