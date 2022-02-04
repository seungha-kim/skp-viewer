#pragma once

#include "RenderMaterial.h"
#include "../scene/Scene.h"
#include "SurfaceInfo.h"

namespace acon {

struct RenderContext {
    const Scene& scene;
    const float playbackValue;
    const SurfaceInfo& surfaceInfo;
};

}
