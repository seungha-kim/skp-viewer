#include "Window.h"
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* glfwWindow, double xPosD, double yPosD) {
    Window& window = *(Window*)glfwGetWindowUserPointer(glfwWindow);
    if (!window.m_cameraRotateMode) return;
    auto xPos = (float)xPosD, yPos = (float)yPosD;
    static float lastX = xPos, lastY = yPos;
    static float sensitivity = 0.1f;
    float xOffset = xPos - lastX, yOffset = yPos - lastY;

    auto& cs = window.m_cameraState;
    cs.yaw += xOffset * sensitivity;
    cs.pitch -= yOffset * sensitivity;

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
    auto& co = m_cameraState;
    auto* window = m_glfwWindow;
    float cameraDelta = co.speed * m_deltaTime;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        co.pos += co.front() * cameraDelta;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        co.pos -= co.front() * cameraDelta;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        co.pos -= glm::normalize(glm::cross(co.front(), co.up)) * cameraDelta;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        co.pos += glm::normalize(glm::cross(co.front(), co.up)) * cameraDelta;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        co.pos -= co.up * cameraDelta;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        co.pos += co.up * cameraDelta;
    std::cout << cameraDelta << std::endl;
    std::cout << co.pos.x << ", "<< co.pos.y << ", "<< co.pos.z << ", " << std::endl;
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

CameraState &Window::cameraState() {
    return m_cameraState;
}

PlaybackState &Window::playbackState() {
    return m_playbackState;
}

InputState &Window::inputState() {
    return m_inputState;
}

