#pragma once

#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../engine/Engine.h"
#include "gui/Gui.h"
#include <memory>

class Window {
    friend void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    friend void window_size_callback(GLFWwindow* window, int width, int height);
    friend void content_scale_size_callback(GLFWwindow* window, float x, float y);
    friend void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
    friend void mouse_wheel_callback(GLFWwindow* window, double xOffset, double yOffset);

public:
    static void initGlfw();
    static void initGl();
    Window(int width, int height, const char* title);
    ~Window();
    void bind();
    bool shouldClose();
    void updateTime();
    void updateCamera();
    void updateKeyCommandSet();
    void swapBuffers();
    void waitEvents();
    float playbackValue();
    void mainLoop();
    void initGlfwGui();

    void beginGui();
    void endGui();

private:
    GLFWwindow* m_glfwWindow = nullptr;
    std::unique_ptr<Engine> m_engine;
    Gui m_gui;

    // Input
    float m_mousePosX;
    float m_mousePosY;
    std::optional<MouseEvent> m_mouseEvent;
    InputController m_inputController;
    PlaybackState m_playbackState;
    KeyCommandSet m_keyCommandSet;
    KeyCommandSet m_prevKeyCommandSet;
    bool m_showMouseCursor = true;
    bool m_shouldClose = false;

    void onMouseMove(float mousePosX, float mousePosY);
    void onMouseWheel(float mousePosX, float mousePosY, float wheelOffsetX, float wheelOffsetY);
    void onKeyboardStateChange(const KeyCommandSet& keyCommandSet);
    void handleInput();
};
