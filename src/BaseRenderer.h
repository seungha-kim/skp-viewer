#pragma once

#include "graphics/Material.h"
#include "Scene.h"
#include "WindowDimension.h"

struct RenderContext {
    const Scene& scene;
    const float playbackValue;
    const Material globalMaterial;
    const WindowDimension& windowDimension;
};

class BaseRenderer {
public:
    virtual ~BaseRenderer() = default;
    virtual void render(RenderContext&) = 0;
};
