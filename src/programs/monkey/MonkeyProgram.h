#pragma once
#include <glad/glad.h>
#include "../../Program.h"
#include "MonkeyRenderer.h"
#include <memory>

class MonkeyProgram final: public Program {
public:
    MonkeyProgram();
    ~MonkeyProgram() override = default;

private:
    void render(RenderContext &ctx) override;

    void processGui(GuiContext &ctx) override;

private:
    std::unique_ptr<MonkeyRenderer> m_renderer;
    GLuint assistantTextureName{};
};
