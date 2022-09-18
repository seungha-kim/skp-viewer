#pragma once
#include <glm/glm.hpp>

namespace acon {

using ObjectId = unsigned;
using UnitId = unsigned;
using MaterialId = unsigned;
using TextureId = unsigned;
using TagId = unsigned;

static const ObjectId ROOT_OBJECT_ID = 0;

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 faceNormal;
    glm::vec2 frontTexCoord {};
    glm::vec2 backTexCoord {};
};

struct Edge {
    glm::vec3 startVertex;
    glm::vec3 endVertex;
    bool soft;
    bool smooth;
};

struct Triangle {
    Vertex vertices[3];
};

struct BoundingBox {
    glm::vec3 min;
    glm::vec3 max;

    [[nodiscard]] glm::vec3 center() const { return (min + max) * 0.5f; }
};

}
