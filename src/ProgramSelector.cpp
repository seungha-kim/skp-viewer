#include "ProgramSelector.h"
#include "TestRenderer.h"

ProgramSelector::ProgramSelector(): m_renderer(std::make_unique<TestRenderer>(true)) {}

void ProgramSelector::renderProgram(RenderContext &ctx) {
    if (m_renderer) {
        m_renderer->render(ctx);
    }
}

void ProgramSelector::changeProgram(ProgramKind kind) {
    switch (kind) {
        case ProgramKind::Test:
            m_renderer = std::make_unique<TestRenderer>(true);
            break;
        case ProgramKind::Test2:
            m_renderer = std::make_unique<TestRenderer>(false);
            break;
    }
}
