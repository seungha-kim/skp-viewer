#include "TextureRenderer.h"

namespace acon {

static const float vertices[] {
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // 1
    1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, // 2
    1.0f,  1.0f,  0.0f, 1.0f, 1.0f, // 3
    1.0f,  1.0f,  0.0f, 1.0f, 1.0f, // 4
    -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, // 5
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // 6
};

TextureRenderer::TextureRenderer()
        : TextureRenderer(std::make_unique<Shader>("quad.vert", "quad.frag")) { }

TextureRenderer::TextureRenderer(std::unique_ptr<Shader> shader)
        : m_shader(std::move(shader)) {
    // VAO
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // VBO
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

TextureRenderer::~TextureRenderer() {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
}

void TextureRenderer::setSourceTexture(const ColorTexture& texture, int index) {
    m_textureNames[index] = texture.textureName();
    m_maxTextureIndex = index > m_maxTextureIndex ? index : m_maxTextureIndex;
}

void TextureRenderer::render(RenderContext& ctx, const std::function<void(Shader&)>& shaderCallback) {
    glViewport(0, 0, ctx.surfaceInfo.physicalWidth, ctx.surfaceInfo.physicalHeight);
    //    glClear(GL_DEPTH_BUFFER_BIT);
    m_shader->use();
    // TODO: remove
    m_shader->setInt("tex", 0);

    if (shaderCallback) {
        shaderCallback(*m_shader);
    }

    for (int i = 0; i <= m_maxTextureIndex; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_textureNames[i]);
    }

    glBindVertexArray(m_vao);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
}

}
