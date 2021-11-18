#include "ProgramSelector.h"
#include "TestRenderer.h"
#include "programs/monkey/MonkeyProgram.h"
#include "RenderOnlyProgram.h"

ProgramSelector::ProgramSelector()
    : m_currentProgram(std::make_unique<MonkeyProgram>()) {}

void ProgramSelector::changeProgram(ProgramKind kind) {
    switch (kind) {
        case ProgramKind::Test:
            m_currentProgram = RenderOnlyProgram::make(std::make_unique<TestRenderer>(true));
            break;
        case ProgramKind::Test2:
            m_currentProgram = RenderOnlyProgram::make(std::make_unique<TestRenderer>(false));
            break;
        case ProgramKind::Monkey:
            m_currentProgram = std::make_unique<MonkeyProgram>();
            break;
    }
}

Program& ProgramSelector::currentProgram() {
    return *m_currentProgram;
}
