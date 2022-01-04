#include "RenderMesh.h"
#include "RenderVertex.h"
#include "../checkError.h"
#include <glm/geometric.hpp>

static RenderVertex convertVertex(const Vertex& vertex) {
    return {
        .pos = vertex.position,
        .normal = vertex.normal,
    };
}

RenderMesh::RenderMesh(const AbstractReader& model, UnitId id, glm::mat4 transform) {
    std::vector<RenderVertex> vertices;
    unsigned faceCount = model.getUnitFaceCount(id);
    for (int i = 0; i < faceCount; i++) {
        auto face = model.getUnitFace(id, i);
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(RenderVertex), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(RenderVertex), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
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

int RenderMesh::verticesCount() const {
    return m_verticesCount;
}

GLuint RenderMesh::VAO() const {
    return m_VAO;
}
