#pragma once

#include "graphics/Material.h"
#include "CameraState.h"

struct RenderContext {
    const CameraState &cam;
    const float playbackValue;
    const Material globalMaterial;
};

class BaseRenderer {
public:
    virtual ~BaseRenderer() = default;
    virtual void render(RenderContext&) = 0;
};
