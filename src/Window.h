#pragma once

#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Engine.h"
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
    void processKeyboardInput();
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
    KeyCommandSet m_keyCommandSet;
    KeyCommandSet m_prevKeyCommandSet;
    bool m_shouldClose = false;
    bool m_showMouseCursor = true;
};
