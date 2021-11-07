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
    if (!window.m_cameraRotateMode) return;
    auto xPos = (float)xPosD, yPos = (float)yPosD;
    static float lastX = xPos, lastY = yPos;
    static float sensitivity = 0.1f;
    float xOffset = xPos - lastX, yOffset = yPos - lastY;

    auto& cam = window.cameraManager().activeCameraMut();
    cam.yaw += xOffset * sensitivity;
    cam.pitch -= yOffset * sensitivity;

    lastX = xPos;
    lastY = yPos;
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
    m_currentTime = (float)glfwGetTime();
    m_deltaTime = m_currentTime - m_lastTime;
    m_lastTime = m_currentTime;
}

void Window::processInput() {
    auto& cam = m_cameraManager.activeCameraMut();
    auto* window = m_glfwWindow;
    float cameraDelta = cam.speed * m_deltaTime;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cam.pos += cam.front() * cameraDelta;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cam.pos -= cam.front() * cameraDelta;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cam.pos -= glm::normalize(glm::cross(cam.front(), cam.up)) * cameraDelta;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cam.pos += glm::normalize(glm::cross(cam.front(), cam.up)) * cameraDelta;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cam.pos -= cam.up * cameraDelta;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        cam.pos += cam.up * cameraDelta;

    // TODO: static
    static int prevSpace = GLFW_RELEASE;
    int currentSpace = glfwGetKey(window, GLFW_KEY_SPACE);
    if (prevSpace == GLFW_RELEASE && currentSpace == GLFW_PRESS) {
        if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            m_cameraRotateMode = false;
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            m_cameraRotateMode = true;
        }
    }
    prevSpace = currentSpace;
}

void Window::swapBuffers() {
    glfwSwapBuffers(m_glfwWindow);
}

void Window::waitEvents() {
    if (m_playbackState.continuous) {
        glfwPostEmptyEvent();
    }
    glfwWaitEvents();

}

float Window::playbackValue() {
    return m_playbackState.manual ? m_playbackState.playback : m_currentTime;
}

float Window::deltaTime() const {
    return m_deltaTime;
}

PlaybackState &Window::playbackState() {
    return m_playbackState;
}

InputState &Window::inputState() {
    return m_inputState;
}

CameraManager &Window::cameraManager() {
    return m_cameraManager;
}

