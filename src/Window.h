#pragma once

#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "PlaybackState.h"
#include "InputState.h"
#include "CameraManager.h"

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
    PlaybackState& playbackState();
    InputState& inputState();
    CameraManager& cameraManager();

private:
    GLFWwindow* m_glfwWindow = nullptr;
    float m_currentTime = 0.0f;
    float m_deltaTime = 0.0f;
    float m_lastTime = 0.0f;
    // TODO: input manager -> flyCameraContext
    bool m_cameraRotateMode = false;
    CameraManager m_cameraManager;

    PlaybackState m_playbackState;
    InputState m_inputState;

};
