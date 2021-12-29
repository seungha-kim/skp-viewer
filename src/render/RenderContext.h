#pragma once

#include "../model/Material.h"
#include "../scene/Scene.h"
#include "SurfaceInfo.h"

struct RenderContext {
    const Scene& scene;
    const float playbackValue;
    const Material globalMaterial;
    const SurfaceInfo& surfaceInfo;
};
