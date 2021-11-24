#pragma once
#include <memory>
#include "Program.h"
#include "SurfaceInfo.h"

enum class ProgramKind {
    Test,
    Test2,
    Monkey
};

class ProgramSelector {
public:
    explicit ProgramSelector(const SurfaceInfo& surfaceInfo);
    void changeProgram(const SurfaceInfo& surfaceInfo, ProgramKind kind);
    Program& currentProgram();
private:
    std::unique_ptr<Program> m_currentProgram;
};
