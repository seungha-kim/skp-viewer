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
    };
}

RenderMesh::RenderMesh(const AbstractReader& model, UnitId id, glm::mat4 transform) {
    // TODO: Material 처리 조금 더 효율적으로 - 여기서 reader 내용을 바로 읽어오는 것이 아니라, 중간계층을 두고 재사용할 수 있는 것은 재사용하기.
    bool hasBackColor = false;
    if (auto backMaterialId = model.getUnitBackMaterial(id)) {
        m_frontColor = m_backColor = model.getMaterialColor(backMaterialId.value());
        hasBackColor = true;
    }
    if (auto frontMaterialId = model.getUnitFrontMaterial(id)) {
        m_frontColor = model.getMaterialColor(frontMaterialId.value());
        if (!hasBackColor) {
            m_backColor = m_frontColor;
        }
    }

    std::vector<RenderVertex> vertices;
    unsigned faceCount = model.getUnitTriangleCount(id);
    for (int i = 0; i < faceCount; i++) {
        auto face = model.getUnitTriangle(id, i);
        for (const auto& vertex : face.vertices) {
            vertices.push_back(convertVertex(vertex));
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

}
