#pragma once
#include <glm/glm.hpp>

namespace acon {

using ObjectId = unsigned;
using UnitId = unsigned;
using MaterialId = unsigned;
using TextureId = unsigned;

static const ObjectId ROOT_OBJECT_ID = 0;

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 faceNormal;
    glm::vec2 frontTexCoord{};
    glm::vec2 backTexCoord{};
};

struct Triangle {
    Vertex vertices[3];
};

}
