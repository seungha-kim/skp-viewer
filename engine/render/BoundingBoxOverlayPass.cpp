#include "BoundingBoxOverlayPass.h"
#include "OffscreenRenderTarget.h"
#include "Shader.h"
#include "checkError.h"

namespace acon {

class BoundingBoxOverlayPassPimpl {
    friend class BoundingBoxOverlayPass;
    std::unique_ptr<Shader> m_mainShader;
    OffscreenRenderTarget m_offscreenRenderTarget;
    std::unique_ptr<ColorTexture> m_colorTexture;

public:
    explicit BoundingBoxOverlayPassPimpl(const SurfaceInfo& surfaceInfo)
            : m_mainShader(std::make_unique<Shader>("boundingBoxEdge.vert", "boundingBoxEdge.frag")) {
        resizeResources(surfaceInfo);
    }

    BoundingBoxOverlayPassOutput render(RenderContext& ctx, const BoundingBoxOverlayPassInput& input) {
        auto& cam = ctx.scene.cameraState();

        m_offscreenRenderTarget.setTargetColorTexture(input.colorTexture, 0);
        m_offscreenRenderTarget.setTargetDepthTexture(input.depthTexture);
        auto binding = m_offscreenRenderTarget.bindAndPrepare(
            std::nullopt, ctx.surfaceInfo.physicalWidth, ctx.surfaceInfo.physicalHeight);

        m_mainShader->use();
        m_mainShader->setMatrix4f("view", cam.viewMatrix());
        m_mainShader->setMatrix4f("projection", cam.projectionMatrix());

        for (const auto* unit: input.units) {
            glBindVertexArray(unit->bboxEdgeVAO());
            m_mainShader->setMatrix4f("model", unit->transform());
            glDrawArrays(GL_LINES, 0, unit->bboxEdgeCount());
        }

        return {};
    }

    void resizeResources(const SurfaceInfo& surfaceInfo) { }
};

BoundingBoxOverlayPass::BoundingBoxOverlayPass(const SurfaceInfo& surfaceInfo)
        : m_pimpl(std::make_unique<BoundingBoxOverlayPassPimpl>(surfaceInfo)) { }

BoundingBoxOverlayPass::~BoundingBoxOverlayPass() = default;

BoundingBoxOverlayPassOutput
BoundingBoxOverlayPass::render(RenderContext& ctx, const BoundingBoxOverlayPassInput& input) {
    return m_pimpl->render(ctx, input);
}

void BoundingBoxOverlayPass::resizeResources(const SurfaceInfo& surfaceInfo) {
    m_pimpl->resizeResources(surfaceInfo);
}

}
