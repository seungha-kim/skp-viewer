#pragma once
#include <glm/glm.hpp>

struct CameraOption {
    float fovyDeg = 45.0f;
    float aspectWidth = 800.0f;
    float aspectHeight = 600.0f;
    float zNear = 0.1f;
    float zFar = 100.0f;
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    float cameraSpeed = 2.5f;
};


