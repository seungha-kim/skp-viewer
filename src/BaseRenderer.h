#pragma once

#include "graphics/Material.h"
#include "Scene.h"

struct RenderContext {
    const Scene& scene;
    const float playbackValue;
    const Material globalMaterial;
};

class BaseRenderer {
public:
    virtual ~BaseRenderer() = default;
    virtual void render(RenderContext&) = 0;
};
