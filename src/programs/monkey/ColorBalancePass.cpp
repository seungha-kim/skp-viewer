#include "../../Shader.h"
#include "ColorBalancePass.h"
#include "../../OffscreenRenderTarget.h"
#include "../../TextureRenderer.h"

class ColorBalancePassPimpl {
    friend class ColorBalancePass;

    OffscreenRenderTarget m_offscreenRenderTarget;
    ColorTexture m_colorTexture;
    TextureRenderer m_textureRenderer;
    glm::vec3 m_colorBalance{};

public:
    explicit ColorBalancePassPimpl(const SurfaceInfo& surfaceInfo)
            : m_colorTexture(surfaceInfo.physicalWidth, surfaceInfo.physicalHeight)
            , m_textureRenderer(std::make_unique<Shader>("colorBalance.vert", "colorBalance.frag")) {
        m_offscreenRenderTarget.setTargetColorTexture(m_colorTexture, 0);
    }

    ~ColorBalancePassPimpl() = default;

    ColorBalancePassOutput render(RenderContext &ctx, const ColorBalancePassInput &input) {
        const auto viewportWidth = ctx.surfaceInfo.physicalWidth;
        const auto viewportHeight = ctx.surfaceInfo.physicalHeight;
        auto binding = m_offscreenRenderTarget.bindAndPrepare(glm::vec3(1.0f, 0.0f, 1.0f), viewportWidth, viewportHeight);
        m_textureRenderer.setSourceTexture(input.colorTexture, 0);
        m_textureRenderer.render(ctx, [&](Shader& shader) {
            shader.setVector3f("colorBalance", m_colorBalance);
        });
        return {
                .colorTexture = m_colorTexture,
        };

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
