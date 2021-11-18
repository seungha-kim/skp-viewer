#include "RenderOnlyProgram.h"

RenderOnlyProgram::RenderOnlyProgram(std::unique_ptr<BaseRenderer> renderer): m_renderer(std::move(renderer)) {}

void RenderOnlyProgram::render(RenderContext &ctx) {
    m_renderer->render(ctx);
}

void RenderOnlyProgram::processGui(GuiContext &ctx) {}

std::unique_ptr<RenderOnlyProgram> RenderOnlyProgram::make(std::unique_ptr<BaseRenderer> &&renderer) {
    return std::make_unique<RenderOnlyProgram>(std::move(renderer));
}
