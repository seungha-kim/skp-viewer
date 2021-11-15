#pragma once

#include <GLFW/glfw3.h>
#include "CameraManager.h"
#include "PlaybackState.h"

struct InputContext {
    GLFWwindow* glfwWindow;
    CameraManager& cameraManager;
    PlaybackState& playbackState;
    const float mouseX;
    const float mouseY;
    const float scrollOffsetX;
    const float scrollOffsetY;
};
