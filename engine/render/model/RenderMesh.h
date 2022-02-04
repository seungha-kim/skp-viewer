#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include "../../build.h"
#include "RenderMaterial.h"
#include "RenderVertex.h"
#include "RenderTexture.h"

namespace acon {

class RenderMesh {
public:
    RenderMesh(UnitId id,
               const std::vector<RenderVertex>& vertices,
               glm::mat4 transform,
               RenderMaterial frontMaterial,
               RenderMaterial backMaterial,
               std::unordered_map<TextureId, std::unique_ptr<RenderTexture>>& textures);

    ~RenderMesh();

    [[nodiscard]] glm::mat4 transform() const;
    [[nodiscard]] glm::vec3 frontColor() const;
    [[nodiscard]] glm::vec3 backColor() const;
    [[nodiscard]] int verticesCount() const;
    [[nodiscard]] GLuint VAO() const;
    [[nodiscard]] std::optional<GLuint> frontTextureName() const;
    [[nodiscard]] std::optional<GLuint> backTextureName() const;

private:
    UnitId m_id;
    glm::mat4 m_transform{};
    int m_verticesCount{};
    GLuint m_VAO{}, m_VBO{};
    glm::vec3 m_frontColor{};
    glm::vec3 m_backColor{};
    GLuint m_frontTexture = 0;
    GLuint m_backTexture = 0;
};

}