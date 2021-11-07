#pragma once

#include <GLFW/glfw3.h>
#include "CameraManager.h"
#include "PlaybackState.h"

struct InputContext {
    GLFWwindow* glfwWindow;
    CameraManager& cameraManager;
    PlaybackState& playbackState;
    float mouseX;
    float mouseY;
};
