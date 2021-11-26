#include "../../Shader.h"
#include "BloomFilterPass.h"
#include "../../OffscreenRenderTarget.h"
#include "../../TextureRenderer.h"

class BloomFilterPassPimpl {
    friend class BloomFilterPass;

    OffscreenRenderTarget m_offscreenRenderTarget;
    ColorTexture m_colorTexture;
    TextureRenderer m_textureRenderer;

public:
    explicit BloomFilterPassPimpl(const SurfaceInfo& surfaceInfo)
            : m_colorTexture(surfaceInfo.physicalWidth, surfaceInfo.physicalHeight)
            , m_textureRenderer(std::make_unique<Shader>("bloom.vert", "bloom.frag")) {
        m_offscreenRenderTarget.setTargetColorTexture(m_colorTexture);
    }

    ~BloomFilterPassPimpl() = default;

    BloomFilterPassOutput render(RenderContext &ctx, const BloomFilterPassInput &input) {
        const auto viewportWidth = ctx.surfaceInfo.physicalWidth;
        const auto viewportHeight = ctx.surfaceInfo.physicalHeight;
        m_offscreenRenderTarget.bindAndPrepare(glm::vec3(1.0f, 0.0f, 1.0f), viewportWidth, viewportHeight);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, input.colorTexture.textureName());
        m_textureRenderer.setTextureToRender(input.colorTexture);
        m_textureRenderer.render(ctx);
        m_offscreenRenderTarget.unbind();
        return BloomFilterPassOutput {
                .colorTexture = m_colorTexture,
        };

    }
};

BloomFilterPass::BloomFilterPass(const SurfaceInfo &surfaceInfo): m_pimpl(std::make_unique<BloomFilterPassPimpl>(surfaceInfo)) {}

BloomFilterPass::~BloomFilterPass() = default;

BloomFilterPassOutput BloomFilterPass::render(RenderContext &ctx, const BloomFilterPassInput &input) {
    return m_pimpl->render(ctx, input);
}
