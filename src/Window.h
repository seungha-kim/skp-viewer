#pragma once

#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "PlaybackState.h"
#include "CameraManager.h"
#include "InputController.h"

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
    void processKeyboardInput();
    void swapBuffers();
    void waitEvents();
    float playbackValue();
    [[nodiscard]] float deltaTime() const;
    PlaybackState& playbackState();
    CameraManager& cameraManager();
    InputController& inputController();

private:
    GLFWwindow* m_glfwWindow = nullptr;
    float m_currentTime = 0.0f;
    float m_deltaTime = 0.0f;
    float m_lastTime = 0.0f;
    CameraManager m_cameraManager;
    InputController m_inputController;

    PlaybackState m_playbackState;
};
