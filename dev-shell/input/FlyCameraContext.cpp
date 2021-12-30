#include "FlyCameraContext.h"
#include <glm/glm.hpp>

void FlyCameraContext::handleKeyboardInput(InputContext &ctx) {
    updateMoveState(ctx);
    updateCamera(ctx);
}

void FlyCameraContext::handleMouseInput(InputContext &ctx, const MouseMoveEvent& mouseMoveEvent) {
    auto xPos = mouseMoveEvent.x, yPos = mouseMoveEvent.y;
    float xOffset = xPos - m_lastMouseX, yOffset = yPos - m_lastMouseY;

    auto& cam = ctx.cameraManager.activeSceneMut().cameraStateMut();
    cam.yaw += xOffset * m_sensitivity;
    cam.pitch -= yOffset * m_sensitivity;

    m_lastMouseX = xPos;
    m_lastMouseY = yPos;
}

void FlyCameraContext::resetLastMousePos(InputContext& ctx) {
    m_lastMouseX = ctx.mousePosX;
    m_lastMouseY = ctx.mousePosY;
}

void FlyCameraContext::updateMoveState(InputContext &ctx) {
    auto& ps = ctx.playbackState;

    if (ctx.isBeingPressed(KeyCommand::FLY_MODE_FORWARD)) {
        m_moveState |= (unsigned)CameraMoveState::forward;
    } else {
        m_moveState &= ~(unsigned)CameraMoveState::forward;
    }

    if (ctx.isBeingPressed(KeyCommand::FLY_MODE_BACKWARD)) {
        m_moveState |= (unsigned)CameraMoveState::backward;
    } else {
        m_moveState &= ~(unsigned)CameraMoveState::backward;
    }

    if (ctx.isBeingPressed(KeyCommand::FLY_MODE_LEFT)) {
        m_moveState |= (unsigned)CameraMoveState::left;
    } else {
        m_moveState &= ~(unsigned)CameraMoveState::left;
    }

    if (ctx.isBeingPressed(KeyCommand::FLY_MODE_RIGHT)) {
        m_moveState |= (unsigned)CameraMoveState::right;
    } else {
        m_moveState &= ~(unsigned)CameraMoveState::right;
    }

    if (ctx.isBeingPressed(KeyCommand::FLY_MODE_DOWNWARD)) {
        m_moveState |= (unsigned)CameraMoveState::down;
    } else {
        m_moveState &= ~(unsigned)CameraMoveState::down;
    }

    if (ctx.isBeingPressed(KeyCommand::FLY_MODE_UPWARD)) {
        m_moveState |= (unsigned)CameraMoveState::up;
    } else {
        m_moveState &= ~(unsigned)CameraMoveState::up;
    }

    if (m_moveState != 0 && !ps.continuousRenderSession.has_value()) {
        ps.continuousRenderSession = ContinuousRenderSession();
    }
    if (m_moveState == 0 && ps.continuousRenderSession.has_value()) {
        ps.continuousRenderSession.reset();
    }
}

void FlyCameraContext::updateCamera(InputContext &ctx) {
    auto& cam = ctx.cameraManager.activeSceneMut().cameraStateMut();
    auto& ps = ctx.playbackState;
    float cameraDelta = 0.0f;
    if (ps.continuousRenderSession.has_value()) {
        cameraDelta = cam.speed * ps.continuousRenderSession.value().deltaTime;
    }

    // TODO: 위쪽 코드랑 중복?
    if (ctx.isBeingPressed(KeyCommand::FLY_MODE_FORWARD)) {
        cam.pos += cam.front() * cameraDelta;
    }

    if (ctx.isBeingPressed(KeyCommand::FLY_MODE_BACKWARD)) {
        cam.pos -= cam.front() * cameraDelta;
    }

    if (ctx.isBeingPressed(KeyCommand::FLY_MODE_LEFT)) {
        cam.pos -= glm::normalize(glm::cross(cam.front(), cam.up)) * cameraDelta;
    }

    if (ctx.isBeingPressed(KeyCommand::FLY_MODE_RIGHT)) {
        cam.pos += glm::normalize(glm::cross(cam.front(), cam.up)) * cameraDelta;
    }

    if (ctx.isBeingPressed(KeyCommand::FLY_MODE_UPWARD)) {
        cam.pos += cam.up * cameraDelta;
    }

    if (ctx.isBeingPressed(KeyCommand::FLY_MODE_DOWNWARD)) {
        cam.pos -= cam.up * cameraDelta;
    }
}
