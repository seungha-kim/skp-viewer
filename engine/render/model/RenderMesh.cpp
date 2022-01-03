#include "RenderMesh.h"
#include "RenderVertex.h"
#include "../checkError.h"
#include <glm/geometric.hpp>

RenderMesh::RenderMesh(aiMesh &mesh, aiMatrix4x4 transform) {
    std::vector<RenderVertex> vertices;

    for (int i = 0; i < mesh.mNumFaces; i++) {
        auto &face = mesh.mFaces[i];
        assert(face.mNumIndices == 3);
        auto vertex1 = vec3AssimpToGlm(mesh.mVertices[face.mIndices[0]]);
        auto vertex2 = vec3AssimpToGlm(mesh.mVertices[face.mIndices[1]]);
        auto vertex3 = vec3AssimpToGlm(mesh.mVertices[face.mIndices[2]]);
        auto normal1 = vec3AssimpToGlm(mesh.mNormals[face.mIndices[0]]);
        auto normal2 = vec3AssimpToGlm(mesh.mNormals[face.mIndices[1]]);
        auto normal3 = vec3AssimpToGlm(mesh.mNormals[face.mIndices[2]]);
//        auto normal = glm::normalize(glm::cross(vertex2 - vertex1, vertex3 - vertex1));
        vertices.push_back(RenderVertex{vertex1, normal1});
        vertices.push_back(RenderVertex{vertex2, normal2});
        vertices.push_back(RenderVertex{vertex3, normal3});
    }
    m_verticesCount = vertices.size();

    // TODO: transform stack
    m_transform = mat4AssimpToGlm(transform);

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

glm::vec3 vec3AssimpToGlm(aiVector3D ai) {
    return {ai.x, ai.y, ai.z};
}

glm::mat4 mat4AssimpToGlm(aiMatrix4x4 ai) {
    glm::mat4 result{1.0f};
    return result; // TODO
}
