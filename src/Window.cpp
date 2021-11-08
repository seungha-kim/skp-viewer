#include "Window.h"
#include <iostream>

void framebuffer_size_callback(GLFWwindow* glfwWindow, int width, int height)
{
    Window& window = *(Window*)glfwGetWindowUserPointer(glfwWindow);
    glViewport(0, 0, width, height);
    auto& cam = window.cameraManager().activeCameraMut();
    cam.aspectWidth = (float)width;
    cam.aspectHeight = (float)height;
}

void mouse_callback(GLFWwindow* glfwWindow, double xPosD, double yPosD) {
    Window& window = *(Window*)glfwGetWindowUserPointer(glfwWindow);
    InputContext ctx {
        .glfwWindow = glfwWindow,
        .cameraManager = window.m_cameraManager,
        .playbackState = window.m_playbackState,
        .mouseX = (float)xPosD,
        .mouseY = (float)yPosD,
    };
    window.m_inputController.handleMouseInput(ctx);
}

void Window::initGlfw() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

}

void Window::initGl() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
    glEnable(GL_DEPTH_TEST);
}

Window::Window(int width, int height, const char *title) {
    if (auto* windowPtr = glfwCreateWindow(width, height, title, NULL, NULL))
    {
        m_glfwWindow = windowPtr;
        glfwSetFramebufferSizeCallback(windowPtr, framebuffer_size_callback);
        glfwSetCursorPosCallback(windowPtr, mouse_callback);
        glfwSetWindowUserPointer(windowPtr, this);
    } else {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
}

Window::~Window() {
    glfwDestroyWindow(m_glfwWindow);
    glfwTerminate();
}

void Window::bind() {
    glfwMakeContextCurrent(m_glfwWindow);
}

GLFWwindow *Window::glfwWindow() const {
    return m_glfwWindow;
}

bool Window::shouldClose() {
    return glfwWindowShouldClose(m_glfwWindow);
}

void Window::updateTime() {
    if (m_playbackState.continuousRenderSession.has_value()) {
        m_playbackState.continuousRenderSession->updateTime();
    }
}

void Window::processKeyboardInput() {
    auto ctx = InputContext {
        .glfwWindow = m_glfwWindow,
        .cameraManager = m_cameraManager,
        .playbackState = m_playbackState,
    };
    m_inputController.handleKeyboardInput(ctx);
}

void Window::swapBuffers() {
    glfwSwapBuffers(m_glfwWindow);
}

void Window::waitEvents() {
    if (m_playbackState.forceContinuous || m_playbackState.continuousRenderSession.has_value()) {
        glfwPostEmptyEvent();
    }
    glfwWaitEvents();
}

float Window::playbackValue() {
    return m_playbackState.manual ? m_playbackState.playback : (float)glfwGetTime();
}

PlaybackState &Window::playbackState() {
    return m_playbackState;
}

CameraManager &Window::cameraManager() {
    return m_cameraManager;
}

InputController &Window::inputController() {
    return m_inputController;
}

