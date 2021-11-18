#pragma once
#include <memory>
#include "Program.h"

enum class ProgramKind {
    Test,
    Test2,
    Monkey
};

class ProgramSelector {
public:
    ProgramSelector();
    void changeProgram(ProgramKind kind);
    Program& currentProgram();
private:
    std::unique_ptr<Program> m_currentProgram;
};
