#pragma once
#include <memory>
#include "BaseRenderer.h"

class GuiContext;
class Program {
public:
    Program() = default;
    virtual ~Program() = default;
    virtual void render(RenderContext& ctx) = 0;
    virtual void processGui(GuiContext& ctx) = 0;
};
