#include "BrightFilterPass.h"
#include "../../Shader.h"
#include "../../OffscreenRenderTarget.h"
#include "../../TextureRenderer.h"

class BrightFilterPassPimpl {
    friend class BrightFilterPass;

    OffscreenRenderTarget m_offscreenRenderTarget;
    ColorTexture m_brightColorTexture;
    TextureRenderer m_textureRenderer;

public:
    explicit BrightFilterPassPimpl(const SurfaceInfo& surfaceInfo)
            : m_brightColorTexture(surfaceInfo.physicalWidth, surfaceInfo.physicalHeight)
            , m_textureRenderer(std::make_unique<Shader>("quad.vert", "brightFilter.frag")) {
        m_offscreenRenderTarget.setTargetColorTexture(m_brightColorTexture, 0);
    }

    ~BrightFilterPassPimpl() = default;

    BrightFilterPassOutput render(RenderContext &ctx, const BrightFilterPassInput &input) {
        const auto viewportWidth = ctx.surfaceInfo.physicalWidth;
        const auto viewportHeight = ctx.surfaceInfo.physicalHeight;
        auto binding = m_offscreenRenderTarget.bindAndPrepare(glm::vec3(0.0f, 0.0f, 0.0f), viewportWidth, viewportHeight);
        m_textureRenderer.setSourceTexture(input.colorTexture, 0);
        m_textureRenderer.render(ctx, [&](Shader& shader) {
            shader.setInt("tex", 0);
            shader.setFloat("threshold", input.threshold);
        });
        return {
            .brightColorTexture = m_brightColorTexture,
        };
    }
};

BrightFilterPass::BrightFilterPass(const SurfaceInfo &surfaceInfo): m_pimpl(std::make_unique<BrightFilterPassPimpl>(surfaceInfo)) {}

BrightFilterPass::~BrightFilterPass() = default;

BrightFilterPassOutput BrightFilterPass::render(RenderContext &ctx, const BrightFilterPassInput &input) {
    return m_pimpl->render(ctx, input);
}
