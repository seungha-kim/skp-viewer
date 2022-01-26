#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "../../reader/AbstractReader.h"

namespace acon {

class RenderMesh {
public:
    RenderMesh(const AbstractReader& model, UnitId id, glm::mat4 transform, GLuint frontTextureName, GLuint backTextureName);

    ~RenderMesh();

    [[nodiscard]] glm::mat4 transform() const;
    [[nodiscard]] glm::vec3 frontColor() const;
    [[nodiscard]] glm::vec3 backColor() const;
    [[nodiscard]] int verticesCount() const;
    [[nodiscard]] GLuint VAO() const;
    [[nodiscard]] std::optional<GLuint> frontTextureName() const;
    [[nodiscard]] std::optional<GLuint> backTextureName() const;

private:
    glm::mat4 m_transform{};
    int m_verticesCount{};
    GLuint m_VAO{}, m_VBO{};
    glm::vec3 m_frontColor{};
    glm::vec3 m_backColor{};
    GLuint m_frontTexture = 0;
    GLuint m_backTexture = 0;
};

}