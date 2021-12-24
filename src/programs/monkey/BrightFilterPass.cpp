#include "BrightFilterPass.h"
#include "../../Shader.h"
#include "../../OffscreenRenderTarget.h"
#include "../../TextureRenderer.h"

class BrightFilterPassPimpl {
    friend class BrightFilterPass;

    OffscreenRenderTarget m_offscreenRenderTarget;
    std::unique_ptr<ColorTexture> m_brightColorTexture;
    TextureRenderer m_textureRenderer;

public:
    explicit BrightFilterPassPimpl(const SurfaceInfo& surfaceInfo)
            : m_textureRenderer(std::make_unique<Shader>("quad.vert", "brightFilter.frag")) {
        resizeResources(surfaceInfo);
    }

    ~BrightFilterPassPimpl() = default;

    BrightFilterPassOutput render(RenderContext &ctx, const BrightFilterPassInput &input) {
        const auto viewportWidth = ctx.surfaceInfo.physicalWidth;
        const auto viewportHeight = ctx.surfaceInfo.physicalHeight;
        m_offscreenRenderTarget.setTargetColorTexture(*m_brightColorTexture, 0);
        auto binding = m_offscreenRenderTarget.bindAndPrepare(glm::vec3(0.0f, 0.0f, 0.0f), viewportWidth, viewportHeight);
        m_textureRenderer.setSourceTexture(input.colorTexture, 0);
        m_textureRenderer.render(ctx, [&](Shader& shader) {
            shader.setInt("tex", 0);
            shader.setFloat("threshold", input.threshold);
        });
        return {
            .brightColorTexture = *m_brightColorTexture,
        };
    }

    void resizeResources(const SurfaceInfo &surfaceInfo) {
        m_brightColorTexture = std::make_unique<ColorTexture>(surfaceInfo.physicalWidth, surfaceInfo.physicalHeight);
    }
};

BrightFilterPass::BrightFilterPass(const SurfaceInfo &surfaceInfo): m_pimpl(std::make_unique<BrightFilterPassPimpl>(surfaceInfo)) {}

BrightFilterPass::~BrightFilterPass() = default;

BrightFilterPassOutput BrightFilterPass::render(RenderContext &ctx, const BrightFilterPassInput &input) {
    return m_pimpl->render(ctx, input);
}

void BrightFilterPass::resizeResources(const SurfaceInfo &surfaceInfo) {
    m_pimpl->resizeResources(surfaceInfo);
}
