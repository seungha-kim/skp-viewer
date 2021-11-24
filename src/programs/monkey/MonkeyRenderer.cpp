#include "MonkeyRenderer.h"
#include "Shader.h"

MonkeyRenderer::MonkeyRenderer(const WindowDimension& dimension,
                               const std::vector<std::unique_ptr<Mesh>>& meshes)
        : m_mainPass{dimension}
        , m_sunlightPass{dimension}
        , m_meshes{meshes} {}

void MonkeyRenderer::render(RenderContext &ctx) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const SunlightPassInput sunlightPassInput {
        .meshes = m_meshes,
    };
    const auto sunlightPassOutput = m_sunlightPass.render(ctx, sunlightPassInput);

    const MainPassInput mainPassInput {
        .meshes = m_meshes,
        .lightSpaceMatrix = sunlightPassOutput.lightSpaceMatrix,
        .depthTexture = sunlightPassOutput.depthTexture,
    };
    const auto mainPassOutput = m_mainPass.render(ctx, mainPassInput);
}

GLuint MonkeyRenderer::assistantTexture() {
    return m_sunlightPass.depthTexture();
}
