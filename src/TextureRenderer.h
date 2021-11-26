#pragma once
#include <glad/glad.h>
#include "ColorTexture.h"
#include "Shader.h"
#include "RenderContext.h"
#include <memory>

class TextureRenderer {
public:
    TextureRenderer();
    TextureRenderer(std::unique_ptr<Shader> shader);
    ~TextureRenderer();
    void setTextureToRender(const ColorTexture& texture);
    void render(RenderContext& ctx);
private:
    std::unique_ptr<Shader> m_shader;
    GLuint m_vao{};
    GLuint m_vbo{};
    GLuint m_textureName{};
};