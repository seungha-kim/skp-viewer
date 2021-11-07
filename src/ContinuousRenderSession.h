#pragma once
#include <GLFW/glfw3.h>

struct ContinuousRenderSession {
    float lastTime = (float)glfwGetTime();
    float deltaTime = 0.0f;

    void updateTime() {
        auto currentTime = (float)glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
    }
};
