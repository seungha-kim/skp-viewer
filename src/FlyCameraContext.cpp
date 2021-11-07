#include "FlyCameraContext.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

void FlyCameraContext::handleKeyboardInput(InputContext &ctx) {
    auto& cam = ctx.cameraManager.activeCameraMut();
    auto* window = ctx.glfwWindow;
    float cameraDelta = cam.speed * ctx.deltaTime;
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
}

void FlyCameraContext::handleMouseInput(InputContext &ctx) {
    auto xPos = ctx.mouseX, yPos = ctx.mouseY;
    static float lastX = xPos, lastY = yPos;
    static float sensitivity = 0.1f;
    float xOffset = xPos - lastX, yOffset = yPos - lastY;

    auto& cam = ctx.cameraManager.activeCameraMut();
    cam.yaw += xOffset * sensitivity;
    cam.pitch -= yOffset * sensitivity;

    lastX = xPos;
    lastY = yPos;
}
