#include "RenderMesh.h"
#include "RenderVertex.h"
#include "../checkError.h"
#include <glm/geometric.hpp>
#include <unordered_map>

namespace acon {

RenderMesh::RenderMesh(UnitId id,
                       const std::vector<RenderVertex>& vertices,
                       glm::mat4 transform,
                       RenderMaterial frontMaterial,
                       RenderMaterial backMaterial,
                       std::unordered_map<TextureId, std::unique_ptr<RenderTexture>>& textures)
        : m_id(id) {

    if (const auto* frontTextureId = std::get_if<TextureId>(&frontMaterial)) {
        m_frontTexture = textures.at(*frontTextureId)->textureName();
    } else if (const auto* frontTextureColor = std::get_if<RenderColor>(&frontMaterial)) {
        m_frontColor = *frontTextureColor;
    }

    if (const auto* backTextureId = std::get_if<TextureId>(&backMaterial)) {
        m_backTexture = textures.at(*backTextureId)->textureName();
    } else if (const auto* backTextureColor = std::get_if<RenderColor>(&backMaterial)) {
        m_backColor = *backTextureColor;
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
