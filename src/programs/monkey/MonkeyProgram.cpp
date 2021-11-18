#include "MonkeyProgram.h"

MonkeyProgram::MonkeyProgram():m_renderer(std::make_unique<MonkeyRenderer>()) {}

void MonkeyProgram::render(RenderContext &ctx) {
    m_renderer->render(ctx);
}

void MonkeyProgram::processGui(GuiContext &ctx) {

}
