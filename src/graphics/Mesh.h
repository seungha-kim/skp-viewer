#pragma once
#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <vector>

glm::vec3 vec3AssimpToGlm(aiVector3D ai);
glm::mat4 mat4AssimpToGlm(aiMatrix4x4 ai);

class Mesh {
public:
    Mesh(aiMesh &mesh, aiMatrix4x4 transform);

    ~Mesh();

    [[nodiscard]] glm::mat4 transform() const;
    [[nodiscard]] int verticesCount() const;
    [[nodiscard]] GLuint VAO() const;

private:
    glm::mat4 m_transform{};
    int m_verticesCount{};
    GLuint m_VAO{}, m_VBO{};
};
