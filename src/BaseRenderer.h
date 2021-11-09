#pragma once

#include "CameraState.h"

struct RenderContext {
    const CameraState &cam;
    const float playbackValue;
};

class BaseRenderer {
public:
    virtual ~BaseRenderer() = default;
    virtual void render(RenderContext&) = 0;
};
