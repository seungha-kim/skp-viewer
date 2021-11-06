#pragma once
#include <glm/glm.hpp>

struct CameraOption {
    float fovyDeg = 45.0f;
    float aspectWidth = 800.0f;
    float aspectHeight = 600.0f;
    float zNear = 0.1f;
    float zFar = 100.0f;
    glm::vec3 pos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    float speed = 2.5f;
};


