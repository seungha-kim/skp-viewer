#pragma once

#include <variant>
#include <optional>
#include <GLFW/glfw3.h>
#include "SceneManager.h"
#include "PlaybackState.h"

struct MouseMoveEvent {
    const float x;
    const float y;
};

struct MouseScrollEvent {
    const float offsetX;
    const float offsetY;
};

using MouseEvent = std::variant<MouseMoveEvent, MouseScrollEvent>;

struct InputContext {
    GLFWwindow* glfwWindow;
    SceneManager& cameraManager;
    PlaybackState& playbackState;
    std::optional<MouseEvent> mouseEvent;
};
