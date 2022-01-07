#pragma once
#include <glm/glm.hpp>

struct DirectionalLight {
    glm::vec3 color {1.0f, 1.0f, 1.0f};
    glm::vec3 direction {-1.0f, 1.0f, -1.0f};
};
