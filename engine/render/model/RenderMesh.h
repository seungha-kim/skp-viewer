#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "../../reader/AbstractReader.h"

namespace acon {

class RenderMesh {
public:
    RenderMesh(const AbstractReader& model, UnitId id, glm::mat4 transform);

    ~RenderMesh();

    [[nodiscard]] glm::mat4 transform() const;
    [[nodiscard]] int verticesCount() const;
    [[nodiscard]] GLuint VAO() const;

private:
    glm::mat4 m_transform{};
    int m_verticesCount{};
    GLuint m_VAO{}, m_VBO{};
};

}