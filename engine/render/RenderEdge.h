#pragma once
#include <glm/glm.hpp>

namespace acon {

struct RenderEdge {
    glm::vec3 pos1;
    glm::vec3 pos2;
    bool soft;
    bool smooth;
};

}

