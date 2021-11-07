#pragma once

#include <GLFW/glfw3.h>
#include "CameraManager.h"

struct InputContext {
    GLFWwindow* glfwWindow;
    CameraManager& cameraManager;
    float deltaTime;
    float mouseX;
    float mouseY;
};
