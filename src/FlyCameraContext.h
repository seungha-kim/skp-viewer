#pragma once

#include "InputContext.h"

enum class CameraMoveState: unsigned {
    forward = 1 << 0,
    backward = 1 << 1,
    left = 1 << 2,
    right = 1 << 3,
    up = 1 << 4,
    down = 1 << 5,
};

class FlyCameraContext {
public:
    void handleKeyboardInput(InputContext& ctx);
    void handleMouseInput(InputContext& ctx);
    [[nodiscard]] bool shouldRenderContinuously() const;
private:
    unsigned m_moveState = 0;
};
