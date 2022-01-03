#pragma once

#include "model/RenderMaterial.h"
#include "../scene/Scene.h"
#include "SurfaceInfo.h"

struct RenderContext {
    const Scene& scene;
    const float playbackValue;
    const RenderMaterial globalMaterial;
    const SurfaceInfo& surfaceInfo;
};
