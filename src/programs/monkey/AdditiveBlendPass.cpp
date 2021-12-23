#include "AdditiveBlendPass.h"
#include "../../Shader.h"
#include "../../OffscreenRenderTarget.h"
#include "../../TextureRenderer.h"

static const char* kindToShader(BlendPassKind kind) {
    switch (kind) {
        case BlendPassKind::additive:
            return "additiveBlend.frag";
        case BlendPassKind::multiplicative:
            return "multiplicativeBlend.frag";
    }
}

class AdditiveBlendPassPimpl {
    friend class AdditiveBlendPass;

    OffscreenRenderTarget m_offscreenRenderTarget;
    ColorTexture m_colorTexture;
    TextureRenderer m_textureRenderer;
    glm::vec3 m_colorBalance{};

public:
    explicit AdditiveBlendPassPimpl(const SurfaceInfo& surfaceInfo, BlendPassKind kind)
            : m_colorTexture(surfaceInfo.physicalWidth, surfaceInfo.physicalHeight)
            , m_textureRenderer(std::make_unique<Shader>("quad.vert", kindToShader(kind))) {
        printf("AdditiveBlendPassPimpl\n");
        m_offscreenRenderTarget.setTargetColorTexture(m_colorTexture, 0);
    }

    ~AdditiveBlendPassPimpl() = default;

    AdditiveBlendPassOutput render(RenderContext &ctx, const AdditiveBlendPassInput &input) {
        const auto viewportWidth = ctx.surfaceInfo.physicalWidth;
        const auto viewportHeight = ctx.surfaceInfo.physicalHeight;
        auto binding = m_offscreenRenderTarget.bindAndPrepare(glm::vec3(0.0f, 0.0f, 0.0f), viewportWidth, viewportHeight);
        m_textureRenderer.setSourceTexture(input.colorTexture1, 0);
        m_textureRenderer.setSourceTexture(input.colorTexture2, 1);
        m_textureRenderer.render(ctx, [&](Shader& shader) {
            shader.setInt("tex1", 0);
            shader.setInt("tex2", 1);
            shader.setFloat("c1", input.c1);
            shader.setFloat("c2", input.c2);
        });
        return {
                .colorTexture = m_colorTexture,
        };
    }
};

AdditiveBlendPass::AdditiveBlendPass(const SurfaceInfo &surfaceInfo, BlendPassKind kind): m_pimpl(std::make_unique<AdditiveBlendPassPimpl>(surfaceInfo, kind)) {}

AdditiveBlendPass::~AdditiveBlendPass() = default;

AdditiveBlendPassOutput AdditiveBlendPass::render(RenderContext &ctx, const AdditiveBlendPassInput &input) {
    return m_pimpl->render(ctx, input);
}
