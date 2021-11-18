#pragma once
#include "BaseRenderer.h"
#include "Program.h"
#include <memory>

class RenderOnlyProgram final: public Program {
public:
    static std::unique_ptr<RenderOnlyProgram> make(std::unique_ptr<BaseRenderer>&& renderer);
    explicit RenderOnlyProgram(std::unique_ptr<BaseRenderer> renderer);
    ~RenderOnlyProgram() override = default;

private:
    void render(RenderContext &ctx) override;

    void processGui(GuiContext &ctx) override;

private:
    std::unique_ptr<BaseRenderer> m_renderer;
};


