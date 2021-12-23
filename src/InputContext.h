#pragma once

#include <variant>
#include <optional>
#include "SceneManager.h"
#include "PlaybackState.h"
#include <bitset>

struct MouseMoveEvent {
    const float x;
    const float y;
};

struct MouseScrollEvent {
    const float offsetX;
    const float offsetY;
};

using MouseEvent = std::variant<MouseMoveEvent, MouseScrollEvent>;

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

using KeyCommandSet = std::bitset<static_cast<unsigned long>(KeyCommand::END_OF_COMMAND)>;

struct InputContext {
    SceneManager& cameraManager;
    PlaybackState& playbackState;
    std::optional<MouseEvent> mouseEvent;
    const KeyCommandSet& keyCommandSet;
    const KeyCommandSet& prevKeyCommandSet;
    bool& shouldClose;
    bool& showMouseCursor;
    float mousePosX;
    float mousePosY;
    bool isBeingPressed(KeyCommand keyCommand);
    bool isJustPressed(KeyCommand keyCommand);
};

struct KeyCommandSetManipulator {
    KeyCommandSet& keyCommandSet;

    void set(KeyCommand keyCommand, bool value);
};