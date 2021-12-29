#pragma once

#include <variant>
#include <optional>
#include "../src/scene/SceneManager.h"
#include "PlaybackState.h"
#include "KeyCommand.h"
#include "KeyCommandSet.h"
#include <bitset>

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
