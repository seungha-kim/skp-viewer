#include "BlendPass.h"
#include "OffscreenRenderTarget.h"
#include "Shader.h"
#include "TextureRenderer.h"

namespace acon {

static const char* makeDefinitions(BlendPassKind kind) {
    switch (kind) {
    case BlendPassKind::additive:
        return "#define FRAG\n#define ADDITIVE_BLEND";
    case BlendPassKind::multiplicative:
        return "#define FRAG\n#define MULTIPLICATIVE_BLEND";
    }
}

class BlendPassPimpl {
    friend class BlendPass;

    OffscreenRenderTarget m_offscreenRenderTarget;
    std::unique_ptr<ColorTexture> m_colorTexture;
    TextureRenderer m_textureRenderer;
    glm::vec3 m_colorBalance {};

public:
    explicit BlendPassPimpl(const SurfaceInfo& surfaceInfo, BlendPassKind kind)
            : m_textureRenderer(
                std::make_unique<Shader>("quad.glsl", "quad.glsl", "#define VERT", makeDefinitions(kind))) {
        resizeResources(surfaceInfo);
    }

    ~BlendPassPimpl() = default;

    BlendPassOutput render(RenderContext& ctx, const BlendPassInput& input) {
        const auto viewportWidth = ctx.surfaceInfo.physicalWidth;
        const auto viewportHeight = ctx.surfaceInfo.physicalHeight;
        m_offscreenRenderTarget.setTargetColorTexture(*m_colorTexture, 0);
        auto binding
            = m_offscreenRenderTarget.bindAndPrepare(glm::vec3(0.0f, 0.0f, 0.0f), viewportWidth, viewportHeight);
        m_textureRenderer.setSourceTexture(input.colorTexture1, 0);
        m_textureRenderer.setSourceTexture(input.colorTexture2, 1);
        m_textureRenderer.render(ctx, [&](Shader& shader) {
            shader.setInt("tex1", 0);
            shader.setInt("tex2", 1);
            shader.setFloat("c1", input.c1);
            shader.setFloat("c2", input.c2);
        });
        return {
            .colorTexture = *m_colorTexture,
        };
    }

    void resizeResources(const SurfaceInfo& surfaceInfo) {
        m_colorTexture = std::make_unique<ColorTexture>(surfaceInfo.physicalWidth, surfaceInfo.physicalHeight, GL_RGB);
    }
};

BlendPass::BlendPass(const SurfaceInfo& surfaceInfo, BlendPassKind kind)
        : m_pimpl(std::make_unique<BlendPassPimpl>(surfaceInfo, kind)) { }

BlendPass::~BlendPass() = default;

BlendPassOutput BlendPass::render(RenderContext& ctx, const BlendPassInput& input) {
    return m_pimpl->render(ctx, input);
}

void BlendPass::resizeResources(const SurfaceInfo& surfaceInfo) {
    m_pimpl->resizeResources(surfaceInfo);
}

}
