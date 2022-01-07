#include "Shader.h"
#include "ColorBalancePass.h"
#include "OffscreenRenderTarget.h"
#include "TextureRenderer.h"

namespace acon {

class ColorBalancePassPimpl {
    friend class ColorBalancePass;

    OffscreenRenderTarget m_offscreenRenderTarget;
    std::unique_ptr<ColorTexture> m_colorTexture;
    TextureRenderer m_textureRenderer;
    glm::vec3 m_colorBalance{};

public:
    explicit ColorBalancePassPimpl(const SurfaceInfo& surfaceInfo)
            : m_textureRenderer(std::make_unique<Shader>("colorBalance.vert", "colorBalance.frag")) {
        resizeResources(surfaceInfo);
    }

    ~ColorBalancePassPimpl() = default;

    ColorBalancePassOutput render(RenderContext &ctx, const ColorBalancePassInput &input) {
        const auto viewportWidth = ctx.surfaceInfo.physicalWidth;
        const auto viewportHeight = ctx.surfaceInfo.physicalHeight;
        m_offscreenRenderTarget.setTargetColorTexture(*m_colorTexture, 0);
        auto binding = m_offscreenRenderTarget.bindAndPrepare(glm::vec3(1.0f, 0.0f, 1.0f), viewportWidth, viewportHeight);
        m_textureRenderer.setSourceTexture(input.colorTexture, 0);
        m_textureRenderer.render(ctx, [&](Shader& shader) {
            shader.setVector3f("colorBalance", m_colorBalance);
        });
        return {
                .colorTexture = *m_colorTexture,
        };

    }

    void resizeResources(const SurfaceInfo &surfaceInfo) {
        m_colorTexture = std::make_unique<ColorTexture>(surfaceInfo.physicalWidth, surfaceInfo.physicalHeight);
    }
};

ColorBalancePass::ColorBalancePass(const SurfaceInfo &surfaceInfo): m_pimpl(std::make_unique<ColorBalancePassPimpl>(surfaceInfo)) {}

ColorBalancePass::~ColorBalancePass() = default;

ColorBalancePassOutput ColorBalancePass::render(RenderContext &ctx, const ColorBalancePassInput &input) {
    return m_pimpl->render(ctx, input);
}

void ColorBalancePass::setColorBalance(glm::vec3 colorBalance) {
    m_pimpl->m_colorBalance = colorBalance;
}

void ColorBalancePass::resizeResources(const SurfaceInfo &surfaceInfo) {
    m_pimpl->resizeResources(surfaceInfo);
}

}
