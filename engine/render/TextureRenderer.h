#pragma once
#include <glad/glad.h>
#include "ColorTexture.h"
#include "Shader.h"
#include "RenderContext.h"
#include <memory>
#include <functional>

namespace acon {

class TextureRenderer {
public:
    TextureRenderer();
    TextureRenderer(std::unique_ptr<Shader> shader);
    ~TextureRenderer();
    void setSourceTexture(const ColorTexture &texture, int index);
    void render(RenderContext& ctx, const std::function<void(Shader&)>& shaderCallback = nullptr);
private:
    std::unique_ptr<Shader> m_shader;
    GLuint m_vao{};
    GLuint m_vbo{};
    GLuint m_textureNames[16] {0};
    int m_maxTextureIndex{};
};

}
