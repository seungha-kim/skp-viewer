#include "OutlinePass.h"
#include "OffscreenRenderTarget.h"
#include "Shader.h"
#include "TextureRenderer.h"

namespace acon {

const GLuint VIEW_BLOCK_BINDING_POINT = 0;

class OutlinePassPimpl {
    friend class OutlinePass;

    OffscreenRenderTarget m_offscreenRenderTarget;
    std::unique_ptr<ColorTexture> m_colorTexture;
    TextureRenderer m_textureRenderer;

public:
    explicit OutlinePassPimpl(const SurfaceInfo& surfaceInfo)
            : m_textureRenderer(std::make_unique<Shader>(
                "quad.glsl",
                "quad.glsl",
                "#define VERT",
                "#define FRAG\n#define OUTLINE",
                std::vector<UniformBinding> {
                    {"ViewBlock", VIEW_BLOCK_BINDING_POINT},
                })) {
        resizeResources(surfaceInfo);
    }

    ~OutlinePassPimpl() = default;

    OutlinePassOutput render(RenderContext& ctx, const OutlinePassInput& input) {
        glBindBufferBase(GL_UNIFORM_BUFFER, VIEW_BLOCK_BINDING_POINT, input.viewBlockBuffer);

        const auto viewportWidth = ctx.surfaceInfo.physicalWidth;
        const auto viewportHeight = ctx.surfaceInfo.physicalHeight;

        m_offscreenRenderTarget.setTargetColorTexture(*m_colorTexture, 0);
        auto binding
            = m_offscreenRenderTarget.bindAndPrepare(glm::vec3(1.0f, 0.0f, 1.0f), viewportWidth, viewportHeight);
        m_textureRenderer.setSourceTexture(input.normalTexture, 0);
        m_textureRenderer.setSourceTexture(input.depthTexture, 1);
        m_textureRenderer.render(ctx, [&](Shader& shader) {
            shader.setInt("normalTexture", 0);
            shader.setInt("depthTexture", 1);
            shader.setInt("lineWidth", input.lineWidth);
            shader.setFloat("zNear", ctx.scene.cameraState().zNear);
            shader.setFloat("zFar", ctx.scene.cameraState().zFar);
        });
        return {
            .colorTexture = *m_colorTexture,
        };
    }

    void resizeResources(const SurfaceInfo& surfaceInfo) {
        m_colorTexture = std::make_unique<ColorTexture>(surfaceInfo.physicalWidth, surfaceInfo.physicalHeight, GL_RGB);
    }
};

OutlinePass::OutlinePass(const SurfaceInfo& surfaceInfo)
        : m_pimpl(std::make_unique<OutlinePassPimpl>(surfaceInfo)) { }

OutlinePass::~OutlinePass() = default;

OutlinePassOutput OutlinePass::render(RenderContext& ctx, const OutlinePassInput& input) {
    return m_pimpl->render(ctx, input);
}

void OutlinePass::resizeResources(const SurfaceInfo& surfaceInfo) {
    m_pimpl->resizeResources(surfaceInfo);
}

}
