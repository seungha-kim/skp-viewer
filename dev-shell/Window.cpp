#include "Window.h"
#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "../engine/reader/AssimpReader.h"

void framebuffer_size_callback(GLFWwindow* glfwWindow, int width, int height)
{
    Window& window = *(Window*)glfwGetWindowUserPointer(glfwWindow);
    auto surfaceInfo = window.m_engine->surfaceInfo();
    surfaceInfo.physicalWidth = width;
    surfaceInfo.physicalHeight = height;
    window.m_engine->resize(surfaceInfo);
}

void window_size_callback(GLFWwindow* glfwWindow, int width, int height)
{
    Window& window = *(Window*)glfwGetWindowUserPointer(glfwWindow);
    auto surfaceInfo = window.m_engine->surfaceInfo();
    surfaceInfo.logicalWidth = width;
    surfaceInfo.logicalHeight = height;
    window.m_engine->resize(surfaceInfo);
}

void content_scale_size_callback(GLFWwindow* glfwWindow, float x, float y)
{
    Window& window = *(Window*)glfwGetWindowUserPointer(glfwWindow);
    auto surfaceInfo = window.m_engine->surfaceInfo();
    surfaceInfo.contentScaleX = x;
    surfaceInfo.contentScaleY = y;
    window.m_engine->resize(surfaceInfo);
}

void mouse_move_callback(GLFWwindow* glfwWindow, double xPosD, double yPosD) {
    Window& window = *(Window*)glfwGetWindowUserPointer(glfwWindow);

    double mousePosX, mousePosY;
    glfwGetCursorPos(glfwWindow, &mousePosX, &mousePosY);

    window.onMouseMove((float) mousePosX, (float) mousePosY);
}

void mouse_wheel_callback(GLFWwindow* glfwWindow, double xOffset, double yOffset) {
    Window& window = *(Window*)glfwGetWindowUserPointer(glfwWindow);

    double mousePosX, mousePosY;
    glfwGetCursorPos(glfwWindow, &mousePosX, &mousePosY);
    window.onMouseWheel((float) mousePosX, (float) mousePosY, (float) xOffset, (float) yOffset);

    // https://github.com/ocornut/imgui/issues/1759
    ImGui_ImplGlfw_ScrollCallback(glfwWindow, xOffset, yOffset);
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
}

Window::Window(int width, int height, const char *title, const acon::AbstractReader& model): m_model(model) {

    initGlfw();
    if (auto* windowPtr = glfwCreateWindow(width, height, title, NULL, NULL))
    {
        acon::SurfaceInfo surfaceInfo;
        m_glfwWindow = windowPtr;
        bind();
        initGl();
        initGlfwGui();
        glfwSetFramebufferSizeCallback(windowPtr, framebuffer_size_callback);
        glfwSetWindowSizeCallback(windowPtr, window_size_callback);
        glfwSetWindowContentScaleCallback(windowPtr, content_scale_size_callback);
        glfwSetCursorPosCallback(windowPtr, mouse_move_callback);
        glfwSetScrollCallback(windowPtr, mouse_wheel_callback);
        glfwSetWindowUserPointer(windowPtr, this);
        glfwGetFramebufferSize(windowPtr, &surfaceInfo.physicalWidth, &surfaceInfo.physicalHeight);
        glfwGetWindowSize(windowPtr, &surfaceInfo.logicalWidth, &surfaceInfo.logicalHeight);
        glfwGetWindowContentScale(windowPtr, &surfaceInfo.contentScaleX, &surfaceInfo.contentScaleY);

        m_engine = std::make_unique<acon::Engine>(surfaceInfo, m_model);
    } else {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
}

Window::~Window() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    glfwDestroyWindow(m_glfwWindow);
    glfwTerminate();
}

void Window::bind() {
    glfwMakeContextCurrent(m_glfwWindow);
}

bool Window::shouldClose() {
    return glfwWindowShouldClose(m_glfwWindow) || m_shouldClose;
}

void Window::updateTime() {
    if (m_playbackState.continuousRenderSession.has_value()) {
        m_playbackState.continuousRenderSession->updateTime((float)glfwGetTime());
    }
}

void Window::updateCamera() {
    auto& cam = m_engine->sceneManagerMut().activeSceneMut().cameraStateMut();
    auto& surfaceInfo = m_engine->surfaceInfo();
    cam.aspectWidth = (float)surfaceInfo.logicalWidth;
    cam.aspectHeight = (float)surfaceInfo.logicalHeight;
}

void Window::updateKeyCommandSet() {
    m_keyCommandSet.setPressed(
            KeyCommand::FLY_MODE_TOGGLE,
            glfwGetKey(m_glfwWindow, GLFW_KEY_SPACE) == GLFW_PRESS
            );
    m_keyCommandSet.setPressed(
            KeyCommand::FLY_MODE_FORWARD,
            glfwGetKey(m_glfwWindow, GLFW_KEY_W) == GLFW_PRESS
            );
    m_keyCommandSet.setPressed(
            KeyCommand::FLY_MODE_BACKWARD,
            glfwGetKey(m_glfwWindow, GLFW_KEY_S) == GLFW_PRESS
            );
    m_keyCommandSet.setPressed(
            KeyCommand::FLY_MODE_LEFT,
            glfwGetKey(m_glfwWindow, GLFW_KEY_A) == GLFW_PRESS
            );
    m_keyCommandSet.setPressed(
            KeyCommand::FLY_MODE_RIGHT,
            glfwGetKey(m_glfwWindow, GLFW_KEY_D) == GLFW_PRESS
            );
    m_keyCommandSet.setPressed(
            KeyCommand::FLY_MODE_UPWARD,
            glfwGetKey(m_glfwWindow, GLFW_KEY_E) == GLFW_PRESS
            );
    m_keyCommandSet.setPressed(
            KeyCommand::FLY_MODE_DOWNWARD,
            glfwGetKey(m_glfwWindow, GLFW_KEY_Q) == GLFW_PRESS
            );
    m_keyCommandSet.setPressed(
            KeyCommand::EXIT,
            glfwGetKey(m_glfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS
            );
    onKeyboardStateChange(m_keyCommandSet);
    // TODO
    if (m_showMouseCursor) {
        glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    } else {
        glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
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

void Window::mainLoop() {
    while (!shouldClose()) {
        updateKeyCommandSet();
        handleInput();

        updateTime();
        updateCamera();
        m_engine->render(playbackValue(), m_selectedObjectIdOpt);

        beginGui();
        GuiContext guiCtx {
                .sceneManager = m_engine->sceneManagerMut(),
                .playbackState = m_playbackState,
                .inputController = m_inputController,
                .surfaceInfo = m_engine->surfaceInfo(),
                .renderOptions = m_engine->rendererMut().renderOptionsMut(),
                .runtimeModel = m_engine->runtimeModelMut(),
                .selectedObjectIdOpt = m_selectedObjectIdOpt,
                .hoveringGui = m_hoveringGui,
        };
        m_gui.process(guiCtx);
        endGui();

        swapBuffers();
        waitEvents();
    }
}

void Window::initGlfwGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(m_glfwWindow, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void Window::beginGui() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Window::endGui() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Window::onMouseMove(float mousePosX, float mousePosY) {
    m_mouseEvent = MouseMoveEvent {.x = mousePosX, .y = (float)mousePosY};
    m_mousePosX = mousePosX;
    m_mousePosY = mousePosY;
}

void Window::onMouseWheel(float mousePosX, float mousePosY, float wheelOffsetX, float wheelOffsetY) {
    m_mouseEvent = MouseScrollEvent {.offsetX = (float)wheelOffsetX, .offsetY = (float)wheelOffsetY};
    m_mousePosX = mousePosX;
    m_mousePosY = mousePosY;
}

void Window::onKeyboardStateChange(const KeyCommandSet& keyCommandSet) {
    m_keyCommandSet = keyCommandSet;
}

void Window::handleInput() {
    InputContext ctx {
            .cameraManager = m_engine->sceneManagerMut(),
            .playbackState = m_playbackState,
            .mouseEvent = m_mouseEvent,
            .keyCommandSet = m_keyCommandSet,
            .prevKeyCommandSet = m_prevKeyCommandSet,
            .shouldClose = m_shouldClose,
            .showMouseCursor = m_showMouseCursor,
            .mousePosX = m_mousePosX,
            .mousePosY = m_mousePosY,
            .disableCameraControlByWheel = m_hoveringGui,
    };
    m_inputController.handleKeyboardInput(ctx);
    m_inputController.handleMouseInput(ctx);

    m_prevKeyCommandSet = m_keyCommandSet;
}
