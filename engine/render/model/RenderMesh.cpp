#include "RenderMesh.h"
#include "RenderVertex.h"
#include "../checkError.h"
#include <glm/geometric.hpp>

namespace acon {

static RenderVertex convertVertex(const Vertex& vertex) {
    return RenderVertex {
        .pos = vertex.position,
        .normal = vertex.normal,
        .faceNormal = vertex.faceNormal,
        .frontTexCoord = vertex.frontTexCoord,
        .backTexCoord = vertex.backTexCoord,
    };
}

static inline glm::vec3 defaultMaterialColor() {
    // TODO: 리더마다 기본값이 다를 수 있음 - e.g. 스케치업 기본 색과 블렌더 기본 색이 다를 수 있음.
    return {0.7f, 0.7f, 0.7f};
}

RenderMesh::RenderMesh(const AbstractReader& model, UnitId id, glm::mat4 transform, GLuint frontTextureName, GLuint backTextureName)
        : m_frontTexture(frontTextureName)
        , m_backTexture(backTextureName) {
    if (auto frontMaterialOpt = model.getUnitFrontMaterial(id)) {
        if (model.getMaterialHasColor(frontMaterialOpt.value())) {
            m_frontColor = model.getMaterialColor(frontMaterialOpt.value());
        }
    } else {
        m_frontColor = defaultMaterialColor();
    }
    if (auto backMaterialOpt = model.getUnitBackMaterial(id)) {
        if (model.getMaterialHasColor(backMaterialOpt.value())) {
            m_backColor = model.getMaterialColor(backMaterialOpt.value());
        }
    } else {
        m_backColor = defaultMaterialColor();
    }

    std::vector<RenderVertex> vertices;
    unsigned faceCount = model.getUnitTriangleCount(id);
    for (int i = 0; i < faceCount; i++) {
        auto face = model.getUnitTriangle(id, i);
        for (const auto& vertex : face.vertices) {
            auto renderVertex = convertVertex(vertex);

            vertices.push_back(renderVertex);
        }
    }
    m_verticesCount = vertices.size();

    // TODO: transform stack
    m_transform = transform;

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(RenderVertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(RenderVertex), (void*)offsetof(RenderVertex, pos));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(RenderVertex), (void*)offsetof(RenderVertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(RenderVertex), (void*)offsetof(RenderVertex, faceNormal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(RenderVertex), (void*)offsetof(RenderVertex, frontTexCoord));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(RenderVertex), (void*)offsetof(RenderVertex, backTexCoord));
    glEnableVertexAttribArray(4);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

RenderMesh::~RenderMesh() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}

glm::mat4 RenderMesh::transform() const {
    return m_transform;
}

glm::vec3 RenderMesh::frontColor() const {
    return m_frontColor;
}

glm::vec3 RenderMesh::backColor() const {
    return m_backColor;
}

int RenderMesh::verticesCount() const {
    return m_verticesCount;
}

GLuint RenderMesh::VAO() const {
    return m_VAO;
}

std::optional<GLuint> RenderMesh::frontTextureName() const {
    if (m_frontTexture) {
        return m_frontTexture;
    } else {
        return {};
    }
}

std::optional<GLuint> RenderMesh::backTextureName() const {
    if (m_backTexture) {
        return m_backTexture;
    } else {
        return {};
    }
}

}
