#include "GaussianBlurPass.h"
#include "OffscreenRenderTarget.h"
#include "TextureRenderer.h"

class GaussianBlurPassPimpl {
    friend class GaussianBlurPass;
public:
    std::unique_ptr<ColorTexture> m_colorTextures[2];
    OffscreenRenderTarget m_offscreenRenderTargets[2];
    TextureRenderer m_textureRenderer;
    bool m_enabled;

    GaussianBlurPassPimpl(const SurfaceInfo& surfaceInfo)
            : m_offscreenRenderTargets{{}, {}}
            , m_textureRenderer(std::make_unique<Shader>("quad.vert", "gaussianBlur.frag")) {
        resizeResources(surfaceInfo);
    }
    ~GaussianBlurPassPimpl() = default;

    GaussianBlurPassOutput render(RenderContext &ctx, const GaussianBlurPassInput& input) {
        if (!m_enabled) {
            return GaussianBlurPassOutput {
                .colorTexture = input.colorTexture,
            };
        }

        const auto viewportWidth = ctx.surfaceInfo.physicalWidth;
        const auto viewportHeight = ctx.surfaceInfo.physicalHeight;
        m_offscreenRenderTargets[0].setTargetColorTexture(*m_colorTextures[0], 0);
        m_offscreenRenderTargets[1].setTargetColorTexture(*m_colorTextures[1], 0);

        bool horizontal = true, first_iteration = true;
        int amount = input.iteration * 2;
        for (unsigned int i = 0; i < amount; i++)
        {
            auto binding = m_offscreenRenderTargets[horizontal].bindAndPrepare(glm::vec3(1.0f, 0.0f, 1.0f), viewportWidth, viewportHeight);
            m_textureRenderer.setSourceTexture(first_iteration ? input.colorTexture : *m_colorTextures[!horizontal], 0);
            m_textureRenderer.render(ctx, [&](Shader& shader) {
                shader.setInt("horizontal", horizontal);
            });
            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }
        return GaussianBlurPassOutput {
            .colorTexture = *m_colorTextures[0],
        };
    }

    void resizeResources(const SurfaceInfo &surfaceInfo) {
        m_colorTextures[0] = std::make_unique<ColorTexture>(surfaceInfo.physicalWidth, surfaceInfo.physicalHeight);
        m_colorTextures[1] = std::make_unique<ColorTexture>(surfaceInfo.physicalWidth, surfaceInfo.physicalHeight);
    }
};

GaussianBlurPass::GaussianBlurPass(const SurfaceInfo &surfaceInfo): m_pimpl(std::make_unique<GaussianBlurPassPimpl>(surfaceInfo)) {}

GaussianBlurPass::~GaussianBlurPass() = default;

GaussianBlurPassOutput GaussianBlurPass::render(RenderContext &ctx, const GaussianBlurPassInput &input) {
    return m_pimpl->render(ctx, input);
}

void GaussianBlurPass::setEnabled(bool enabled) {
    m_pimpl->m_enabled = enabled;
}

void GaussianBlurPass::resizeResources(const SurfaceInfo &surfaceInfo) {
    m_pimpl->resizeResources(surfaceInfo);
}
