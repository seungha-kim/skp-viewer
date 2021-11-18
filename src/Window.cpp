#include "Window.h"
#include <iostream>

void framebuffer_size_callback(GLFWwindow* glfwWindow, int width, int height)
{
    Window& window = *(Window*)glfwGetWindowUserPointer(glfwWindow);
    window.m_dimension.framebufferWidth = width;
    window.m_dimension.framebufferHeight = height;
}

void window_size_callback(GLFWwindow* glfwWindow, int width, int height)
{
    Window& window = *(Window*)glfwGetWindowUserPointer(glfwWindow);
    window.m_dimension.width = width;
    window.m_dimension.height = height;
}

void content_scale_size_callback(GLFWwindow* glfwWindow, float x, float y)
{
    Window& window = *(Window*)glfwGetWindowUserPointer(glfwWindow);
    window.m_dimension.contentScaleX = x;
    window.m_dimension.contentScaleY = y;
}

void mouse_move_callback(GLFWwindow* glfwWindow, double xPosD, double yPosD) {
    Window& window = *(Window*)glfwGetWindowUserPointer(glfwWindow);
    InputContext ctx {
        .glfwWindow = glfwWindow,
        .cameraManager = window.m_sceneManager,
        .playbackState = window.m_playbackState,
        .mouseEvent = MouseMoveEvent {.x = (float)xPosD, .y = (float)yPosD},
    };
    window.m_inputController.handleMouseInput(ctx);
}

void mouse_wheel_callback(GLFWwindow* glfwWindow, double xOffset, double yOffset) {
    Window& window = *(Window*)glfwGetWindowUserPointer(glfwWindow);
    InputContext ctx {
            .glfwWindow = glfwWindow,
            .cameraManager = window.m_sceneManager,
            .playbackState = window.m_playbackState,
            .mouseEvent = MouseScrollEvent {.offsetX = (float)xOffset, .offsetY = (float)yOffset},
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
        glfwSetWindowSizeCallback(windowPtr, window_size_callback);
        glfwSetWindowContentScaleCallback(windowPtr, content_scale_size_callback);
        glfwSetCursorPosCallback(windowPtr, mouse_move_callback);
        glfwSetScrollCallback(windowPtr, mouse_wheel_callback);
        glfwSetWindowUserPointer(windowPtr, this);
        glfwGetFramebufferSize(windowPtr, &m_dimension.framebufferWidth, &m_dimension.framebufferHeight);
        glfwGetWindowSize(windowPtr, &m_dimension.width, &m_dimension.height);
        glfwGetWindowContentScale(windowPtr, &m_dimension.contentScaleX, &m_dimension.contentScaleY);
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

void Window::updateCamera() {
    auto& cam = sceneManagerMut().activeSceneMut().cameraStateMut();
    cam.aspectWidth = (float)m_dimension.width;
    cam.aspectHeight = (float)m_dimension.height;
}

void Window::processKeyboardInput() {
    auto ctx = InputContext {
        .glfwWindow = m_glfwWindow,
        .cameraManager = m_sceneManager,
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

const PlaybackState &Window::playbackState() const {
    return m_playbackState;
}

PlaybackState &Window::playbackStateMut() {
    return m_playbackState;
}

const SceneManager &Window::sceneManager() const {
    return m_sceneManager;
}

SceneManager &Window::sceneManagerMut() {
    return m_sceneManager;
}

const InputController &Window::inputController() const {
    return m_inputController;
}

InputController &Window::inputControllerMut() {
    return m_inputController;
}

const WindowDimension &Window::dimension() const {
    return m_dimension;
}


