#include "../Shader.h"
#include "ToneMapPass.h"
#include "../OffscreenRenderTarget.h"
#include "../TextureRenderer.h"

class ToneMapPassPimpl {
    friend class ToneMapPass;

    OffscreenRenderTarget m_offscreenRenderTarget;
    std::unique_ptr<ColorTexture> m_colorTexture;
    TextureRenderer m_textureRenderer;

public:
    explicit ToneMapPassPimpl(const SurfaceInfo& surfaceInfo)
            : m_textureRenderer(std::make_unique<Shader>("quad.vert", "toneMap.frag")) {
        resizeResources(surfaceInfo);
    }

    ~ToneMapPassPimpl() = default;

    ToneMapPassOutput render(RenderContext &ctx, const ToneMapPassInput &input) {
        if (!input.enabled) {
            return {
                    .colorTexture = input.colorTexture,
            };
        }
        const auto viewportWidth = ctx.surfaceInfo.physicalWidth;
        const auto viewportHeight = ctx.surfaceInfo.physicalHeight;
        m_offscreenRenderTarget.setTargetColorTexture(*m_colorTexture, 0);
        auto binding = m_offscreenRenderTarget.bindAndPrepare(glm::vec3(0.0f, 0.0f, 0.0f), viewportWidth, viewportHeight);
        m_textureRenderer.setSourceTexture(input.colorTexture, 0);
        m_textureRenderer.render(ctx, [&](Shader& shader) {
            shader.setFloat("exposure", input.exposure);
            shader.setFloat("gamma", input.gamma);
        });
        return {
                .colorTexture = *m_colorTexture,
        };
    }

    void resizeResources(const SurfaceInfo &surfaceInfo) {
        m_colorTexture = std::make_unique<ColorTexture>(surfaceInfo.physicalWidth, surfaceInfo.physicalHeight);
    }
};

ToneMapPass::ToneMapPass(const SurfaceInfo &surfaceInfo): m_pimpl(std::make_unique<ToneMapPassPimpl>(surfaceInfo)) {}

ToneMapPass::~ToneMapPass() = default;

ToneMapPassOutput ToneMapPass::render(RenderContext &ctx, const ToneMapPassInput &input) {
    return m_pimpl->render(ctx, input);
}

void ToneMapPass::resizeResources(const SurfaceInfo &surfaceInfo) {
    m_pimpl->resizeResources(surfaceInfo);
}