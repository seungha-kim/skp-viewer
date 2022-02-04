#pragma once
#include <glm/glm.hpp>

namespace acon {

struct RenderVertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec3 faceNormal;
    glm::vec2 frontTexCoord;
    glm::vec2 backTexCoord;
};

}
