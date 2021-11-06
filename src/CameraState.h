#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct CameraState {
    float fovyDeg = 45.0f;
    float aspectWidth = 800.0f;
    float aspectHeight = 600.0f;
    float zNear = 0.1f;
    float zFar = 100.0f;
    glm::vec3 pos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    float speed = 2.5f;
    float yaw = -90.0f;
    float pitch = 0.0f;

    [[nodiscard]] glm::vec3 front() const {
        glm::vec3 result;
        result.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
        result.y = sin(glm::radians(pitch));
        result.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
        return result;
    }

    [[nodiscard]] glm::mat4 viewMatrix() const {
        return glm::lookAt(pos, pos + front(), up);
    }

    [[nodiscard]] glm::mat4 projectionMatrix() const {
        return glm::perspective(glm::radians(fovyDeg), aspectWidth / aspectHeight, zNear, zFar);
    }
};


