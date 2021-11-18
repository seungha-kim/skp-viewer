#include "InputController.h"
#include <variant>

void InputController::handleMouseInput(InputContext &ctx) {
    const auto& mouseEvent = ctx.mouseEvent.value();
    if (auto* moveEvent = std::get_if<MouseMoveEvent>(&mouseEvent)) {
        if (m_cameraRotateMode) {
            m_flyCameraContext.handleMouseInput(ctx, *moveEvent);
        }
    } else if (auto* scrollEvent = std::get_if<MouseScrollEvent>(&mouseEvent)) {
        auto& cam = ctx.cameraManager.activeSceneMut().cameraStateMut();
        cam.fovyDeg = glm::clamp(cam.fovyDeg - scrollEvent->offsetY, 10.0f, 170.0f);
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
