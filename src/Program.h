#pragma once
#include <memory>
#include "RenderContext.h"

class GuiContext;
class Program {
public:
    Program() = default;
    virtual ~Program() = default;
    virtual void render(RenderContext& ctx) {};
    virtual void processGui(GuiContext& ctx) {};
    virtual void resizeResources(const SurfaceInfo& surfaceInfo) {};
};
