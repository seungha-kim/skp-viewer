#include "FlyCameraContext.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

void FlyCameraContext::handleKeyboardInput(InputContext &ctx) {
    updateMoveState(ctx);
    updateCamera(ctx);
}

void FlyCameraContext::handleMouseInput(InputContext &ctx) {
    auto xPos = ctx.mouseX, yPos = ctx.mouseY;
    float xOffset = xPos - m_lastMouseX, yOffset = yPos - m_lastMouseY;

    auto& cam = ctx.cameraManager.activeCameraMut();
    cam.yaw += xOffset * m_sensitivity;
    cam.pitch -= yOffset * m_sensitivity;

    m_lastMouseX = xPos;
    m_lastMouseY = yPos;
}

void FlyCameraContext::resetLastMousePos(InputContext& ctx) {
    double xpos, ypos;
    glfwGetCursorPos(ctx.glfwWindow, &xpos, &ypos);
    m_lastMouseX = (float)xpos;
    m_lastMouseY = (float)ypos;
}

void FlyCameraContext::updateMoveState(InputContext &ctx) {
    auto& ps = ctx.playbackState;
    auto* window = ctx.glfwWindow;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        m_moveState |= (unsigned)CameraMoveState::forward;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE) {
        m_moveState &= ~(unsigned)CameraMoveState::forward;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        m_moveState |= (unsigned)CameraMoveState::backward;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE) {
        m_moveState &= ~(unsigned)CameraMoveState::backward;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        m_moveState |= (unsigned)CameraMoveState::left;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE) {
        m_moveState &= ~(unsigned)CameraMoveState::left;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        m_moveState |= (unsigned)CameraMoveState::right;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE) {
        m_moveState &= ~(unsigned)CameraMoveState::right;
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        m_moveState |= (unsigned)CameraMoveState::down;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE) {
        m_moveState &= ~(unsigned)CameraMoveState::down;
    }

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        m_moveState |= (unsigned)CameraMoveState::up;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) {
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
    auto& cam = ctx.cameraManager.activeCameraMut();
    auto& ps = ctx.playbackState;
    auto* window = ctx.glfwWindow;
    float cameraDelta = 0.0f;
    if (ps.continuousRenderSession.has_value()) {
        cameraDelta = cam.speed * ps.continuousRenderSession.value().deltaTime;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cam.pos += cam.front() * cameraDelta;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cam.pos -= cam.front() * cameraDelta;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cam.pos -= glm::normalize(glm::cross(cam.front(), cam.up)) * cameraDelta;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cam.pos += glm::normalize(glm::cross(cam.front(), cam.up)) * cameraDelta;
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        cam.pos -= cam.up * cameraDelta;
    }

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        cam.pos += cam.up * cameraDelta;
    }
}
