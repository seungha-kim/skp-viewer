#pragma once

#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "PlaybackState.h"
#include "SceneManager.h"
#include "InputController.h"
#include "SurfaceInfo.h"

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
    [[nodiscard]] GLFWwindow* glfwWindow() const;
    bool shouldClose();
    void updateTime();
    void updateCamera();
    void processKeyboardInput();
    void swapBuffers();
    void waitEvents();
    float playbackValue();

    [[nodiscard]] const PlaybackState& playbackState() const;
    PlaybackState& playbackStateMut();

    [[nodiscard]] const SceneManager& sceneManager() const;
    SceneManager& sceneManagerMut();

    [[nodiscard]] const InputController& inputController() const;
    InputController& inputControllerMut();

    [[nodiscard]] const SurfaceInfo& surfaceInfo() const;

private:
    GLFWwindow* m_glfwWindow = nullptr;
    SceneManager m_sceneManager;
    InputController m_inputController;

    PlaybackState m_playbackState;
    SurfaceInfo m_surfaceInfo;
};
