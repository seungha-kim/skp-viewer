#include "InputController.h"

void InputController::handleMouseInput(InputContext &ctx) {
    if (m_cameraRotateMode) {
        m_flyCameraContext.handleMouseInput(ctx);
    }
}

void InputController::handleKeyboardInput(InputContext &ctx) {
    auto* window = ctx.glfwWindow;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    int currentSpace = glfwGetKey(window, GLFW_KEY_SPACE);
    if (!m_guiFocused && m_prevSpaceState == GLFW_RELEASE && currentSpace == GLFW_PRESS) {
        if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            m_cameraRotateMode = false;
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            m_cameraRotateMode = true;
            m_flyCameraContext.resetLastMousePos(ctx);
        }
    }
    m_prevSpaceState = currentSpace;

    if (m_cameraRotateMode) {
        m_flyCameraContext.handleKeyboardInput(ctx);
    } else {
        m_objectEditContext.handleKeyboardInput(ctx);
    }
}

void InputController::setGuiFocused(bool value) {
    m_guiFocused = value;
}

bool InputController::isCameraRotateMode() {
    return m_cameraRotateMode;
}

void InputController::handleScrollInput(InputContext &ctx) {
    auto& cam = ctx.cameraManager.activeCameraMut();
    cam.fovyDeg = glm::clamp(cam.fovyDeg + ctx.scrollOffsetY, 10.0f, 170.0f);
}
