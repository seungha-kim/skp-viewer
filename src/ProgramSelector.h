#pragma once
#include <memory>
#include "Program.h"
#include "WindowDimension.h"

enum class ProgramKind {
    Test,
    Test2,
    Monkey
};

class ProgramSelector {
public:
    explicit ProgramSelector(const WindowDimension& dimension);
    void changeProgram(const WindowDimension& dimension, ProgramKind kind);
    Program& currentProgram();
private:
    std::unique_ptr<Program> m_currentProgram;
};
