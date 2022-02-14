#pragma once

#include "RenderMaterial.h"
#include "../scene/Scene.h"
#include "SurfaceInfo.h"
#include <optional>
#include "RenderModel.h"
#include "../runtime/RuntimeModel.h"
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
