#pragma once
#include <memory>
#include "BaseRenderer.h"

enum class ProgramKind {
    Test,
    Test2,
    Monkey
};

class ProgramSelector {
public:
    ProgramSelector();
    void renderProgram(RenderContext& ctx);
    void changeProgram(ProgramKind kind);
private:
    std::unique_ptr<BaseRenderer> m_renderer;
};
