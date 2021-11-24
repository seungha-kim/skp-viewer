#pragma once
#include <glad/glad.h>
#include "../../Program.h"
#include "MonkeyRenderer.h"
#include <memory>
#include "../../WindowDimension.h"

class MonkeyProgram final: public Program {
public:
    explicit MonkeyProgram(const WindowDimension& dimension);
    ~MonkeyProgram() override = default;

private:
    void render(RenderContext &ctx) override;

    void processGui(GuiContext &ctx) override;

private:
    std::unique_ptr<MonkeyRenderer> m_renderer;
    std::vector<std::unique_ptr<Mesh>> m_meshes;
};
