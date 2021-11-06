#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "CameraState.h"
#include "RenderState.h"
#include "InputState.h"

class Window {
    friend void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    friend void mouse_callback(GLFWwindow* window, double xpos, double ypos);

public:
    static void initGlfw();
    static void initGl();
    Window(int width, int height, const char* title);
    ~Window();
    void bind();
    [[nodiscard]] GLFWwindow* glfwWindow() const;
    bool shouldClose();
    void updateTime();
    void processInput();
    void swapBuffers();
    void waitEvents();
    float playbackValue();
    [[nodiscard]] float deltaTime() const;
    // TODO: 데이터 노출
    CameraState& cameraState();
    RenderState& renderState();
    InputState& inputState();

private:
    GLFWwindow* m_glfwWindow = nullptr;
    float m_currentTime = 0.0f;
    float m_deltaTime = 0.0f;
    float m_lastTime = 0.0f;
    bool m_cameraRotateMode = false;

    CameraState m_cameraState;
    RenderState m_renderState;
    InputState m_inputState;
};
