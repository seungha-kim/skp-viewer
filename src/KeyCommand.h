#pragma once

enum class KeyCommand: unsigned {
    EXIT = 0,

    FLY_MODE_TOGGLE = 10,
    FLY_MODE_FORWARD,
    FLY_MODE_BACKWARD,
    FLY_MODE_LEFT,
    FLY_MODE_RIGHT,
    FLY_MODE_UPWARD,
    FLY_MODE_DOWNWARD,

    END_OF_COMMAND = 256
};
