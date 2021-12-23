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
    if (ctx.isBeingPressed(KeyCommand::EXIT)) {
        ctx.shouldClose = true;
    }

    if (!m_guiFocused && ctx.isJustPressed(KeyCommand::FLY_MODE_TOGGLE )) {
        if (m_cameraRotateMode) {
            m_cameraRotateMode = false;
        } else {
            m_cameraRotateMode = true;
            m_flyCameraContext.resetLastMousePos(ctx);
        }
        ctx.showMouseCursor = !m_cameraRotateMode;
    }

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
