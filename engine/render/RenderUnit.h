#pragma once
#include "../build.h"
#include "RenderMaterial.h"
#include "RenderTexture.h"
#include "RenderVertex.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>

namespace acon {

struct RenderUnitTempData;
class RenderUnit {
public:
    RenderUnit(
        UnitId id,
        ObjectId objectId,
        std::vector<RenderVertex>&& vertices,
        BoundingBox boundingBox,
        glm::mat4 transform,
        RenderMaterial frontMaterial,
        RenderMaterial backMaterial,
        std::unordered_map<TextureId, std::unique_ptr<RenderTexture>>& textures);

    ~RenderUnit();

    [[nodiscard]] const ObjectId& objectId() const;
    [[nodiscard]] glm::mat4 transform() const;
    [[nodiscard]] glm::vec3 frontColor() const;
    [[nodiscard]] glm::vec3 backColor() const;
    [[nodiscard]] int verticesCount() const;
    [[nodiscard]] GLuint VAO() const;
    [[nodiscard]] GLuint bboxEdgeVAO() const;
    [[nodiscard]] std::optional<GLuint> frontTextureName() const;
    [[nodiscard]] std::optional<GLuint> backTextureName() const;
    [[nodiscard]] BoundingBox boundingBox() const;
    [[nodiscard]] glm::vec3 worldSpaceCenter() const;
    [[nodiscard]] static inline constexpr unsigned bboxEdgeCount() { return 12 * 2; };
    [[nodiscard]] bool useOpacity() const;
    [[nodiscard]] float frontOpacity() const;
    [[nodiscard]] float backOpacity() const;
    void prepareToRender();

private:
    UnitId m_id;
    ObjectId m_objectId;
    glm::mat4 m_transform {};
    int m_verticesCount {};
    GLuint m_VAO {}, m_VBO {};
    GLuint m_bboxEdgeVAO {}, m_bboxEdgeVBO {};
    glm::vec3 m_frontColor {};
    glm::vec3 m_backColor {};
    GLuint m_frontTexture = 0;
    GLuint m_backTexture = 0;
    bool m_frontUseOpacity = false;
    float m_frontOpacity = 1.0f;
    bool m_backUseOpacity = false;
    float m_backOpacity = 1.0f;
    // TODO: RenderMaterial
    BoundingBox m_boundingBox;
    std::unique_ptr<RenderUnitTempData> m_tempData;
};

}