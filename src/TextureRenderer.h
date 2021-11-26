#pragma once
#include <glad/glad.h>
#include "ColorTexture.h"
#include "Shader.h"
#include "RenderContext.h"

class TextureRenderer {
public:
    TextureRenderer();
    ~TextureRenderer();
    void setTexture(const ColorTexture& texture);
    void render(RenderContext& ctx);
private:
    Shader m_shader;
    GLuint m_vao{};
    GLuint m_vbo{};
    GLuint m_textureName{};
};