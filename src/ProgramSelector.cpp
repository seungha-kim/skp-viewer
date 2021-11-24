#include "ProgramSelector.h"
#include "TestProgram.h"
#include "programs/monkey/MonkeyProgram.h"

ProgramSelector::ProgramSelector(const SurfaceInfo& surfaceInfo)
    : m_currentProgram(std::make_unique<MonkeyProgram>(surfaceInfo)) {}

void ProgramSelector::changeProgram(const SurfaceInfo& surfaceInfo, ProgramKind kind) {
    switch (kind) {
        case ProgramKind::Test:
            m_currentProgram = std::make_unique<TestProgram>(true);
            break;
        case ProgramKind::Test2:
            m_currentProgram = std::make_unique<TestProgram>(false);
            break;
        case ProgramKind::Monkey:
            m_currentProgram = std::make_unique<MonkeyProgram>(surfaceInfo);
            break;
    }
}

Program& ProgramSelector::currentProgram() {
    return *m_currentProgram;
}
