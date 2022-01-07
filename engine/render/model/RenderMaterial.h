#pragma once
#include <glm/glm.hpp>

namespace acon {

struct RenderMaterial {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

}
