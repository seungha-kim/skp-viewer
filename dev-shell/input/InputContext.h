#pragma once

#include "../../engine/scene/SceneManager.h"
#include "../PlaybackState.h"
#include "KeyCommand.h"
#include "KeyCommandSet.h"
#include <bitset>
#include <optional>
#include <variant>

struct MouseMoveEvent {
    float x;
    float y;
};

struct MouseScrollEvent {
    float offsetX;
    float offsetY;
};

using MouseEvent = std::variant<MouseMoveEvent, MouseScrollEvent>;

struct InputContext {
    acon::SceneManager& cameraManager;
    PlaybackState& playbackState;
    std::optional<MouseEvent> mouseEvent;
    const KeyCommandSet& keyCommandSet;
    const KeyCommandSet& prevKeyCommandSet;
    bool& shouldClose;
    bool& showMouseCursor;
    float mousePosX;
    float mousePosY;
    const bool disableCameraControlByWheel;

    bool isBeingPressed(KeyCommand keyCommand);
    bool isJustPressed(KeyCommand keyCommand);
};
