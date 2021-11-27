
#include "TextureRenderer.h"

static const float vertices[] {
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
};

TextureRenderer::TextureRenderer(): TextureRenderer(std::make_unique<Shader>("quad.vert", "quad.frag")) {}

TextureRenderer::TextureRenderer(std::unique_ptr<Shader> shader): m_shader(std::move(shader)) {
    // VAO
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // VBO
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

TextureRenderer::~TextureRenderer() {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
}

void TextureRenderer::setTextureToRender(const ColorTexture &texture) {
    m_textureName = texture.textureName();
}

void TextureRenderer::render(RenderContext &ctx, const std::function<void(Shader&)>& shaderCallback) {
    glViewport(0, 0, ctx.surfaceInfo.physicalWidth, ctx.surfaceInfo.physicalHeight);
//    glClear(GL_DEPTH_BUFFER_BIT);
    m_shader->use();
    // TODO: remove
    m_shader->setInt("tex", 0);

    if (shaderCallback) {
        shaderCallback(*m_shader);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textureName);

    glBindVertexArray(m_vao);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
}

