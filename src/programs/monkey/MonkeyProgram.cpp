#include "MonkeyProgram.h"
#include <imgui.h>
#include "../../guiCommon.h"

MonkeyProgram::MonkeyProgram():m_renderer(std::make_unique<MonkeyRenderer>()) {}

void MonkeyProgram::render(RenderContext &ctx) {
    m_renderer->render(ctx);
}

void MonkeyProgram::processGui(GuiContext &ctx) {
    auto assistant = m_renderer->assistantTexture();
    ImGui::Begin("Assistant View", nullptr, windowFlag(ctx));
    ImGui::Image((void*)(intptr_t)assistant, ImVec2(512,512), ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();
}
