#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct CameraState {
    float fovyDeg = 45.0f;
    float aspectWidth = 800.0f;
    float aspectHeight = 600.0f;
    float zNear = 0.1f;
    float zFar = 100.0f;
    glm::vec3 pos = glm::vec3(-5.0f, -5.0f, 5.0f);
    glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);
    float speed = 2.5f;
    float pitch = -35.0f;
    float yaw = -50.0f;

    [[nodiscard]] glm::vec3 front() const {
        glm::vec3 result;
        result.x = -cos(glm::radians(pitch)) * sin(glm::radians(yaw));
        result.y = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
        result.z = sin(glm::radians(pitch));
        return result;
    }

    [[nodiscard]] glm::vec3 left() const {
        return glm::cross(up, front());
    }

    [[nodiscard]] glm::mat4 viewMatrix() const {
        return glm::lookAt(pos, pos + front(), up);
    }

    [[nodiscard]] glm::mat4 projectionMatrix() const {
        return glm::perspective(glm::radians(fovyDeg), aspectWidth / aspectHeight, zNear, zFar);
    }

    [[nodiscard]] float fovXRadian() const {
        // tan(fovy/2) / tan(fov_x/2) == height / width
        return atan((float)tan(glm::radians(fovyDeg) / 2.0) * aspectWidth / aspectHeight) * 2.0f;
    }

    [[nodiscard]] glm::vec3 leftBottomNearVs() const {
        const auto fovy_rad = glm::radians(fovyDeg);
        const auto fovx_rad = fovXRadian();
        const auto left = -zNear * tan(fovx_rad / 2.0f);
        const auto bottom = -zNear * tan(fovy_rad / 2.0f);
        return {left, bottom, zNear};
    }

    [[nodiscard]] glm::vec3 viewVec0() const {
        const auto fovy_rad = glm::radians(fovyDeg);
        const auto fovx_rad = fovXRadian();
        const auto left = -tan(fovx_rad / 2.0f);
        const auto bottom = -tan(fovy_rad / 2.0f);
        return {left, bottom, 1.0};
    }

    [[nodiscard]] glm::vec3 viewVec1() const {
        const auto fovy_rad = glm::radians(fovyDeg);
        const auto fovx_rad = fovXRadian();
        const auto right = tan(fovx_rad / 2.0f);
        const auto top = tan(fovy_rad / 2.0f);
        return {right, top, 1.0};
    }

    [[nodiscard]] glm::vec3 rightTopFarVs() const {
        const auto fovy_rad = glm::radians(fovyDeg);
        const auto fovx_rad = fovXRadian();
        const auto right = zFar * tan(fovx_rad / 2.0f);
        const auto top = zFar * tan(fovy_rad / 2.0f);
        return {right, top, zFar};
    }
};


