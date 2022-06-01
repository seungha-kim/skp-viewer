#include "Renderer.h"
#include "checkError.h"
#include <stack>

namespace acon {

Renderer::Renderer(const SurfaceInfo& surfaceInfo)
        : m_sunlightPass(surfaceInfo)
        , m_mainPass(surfaceInfo)
        , m_gaussianBlurPass(surfaceInfo)
        , m_additiveBlendPass(surfaceInfo, BlendPassKind::additive)
        , m_brightFilterPass(surfaceInfo)
        , m_toneMapPass(surfaceInfo)
        , m_outlinePass(surfaceInfo)
        , m_outlineMultiplicativeBlendPass(surfaceInfo, BlendPassKind::multiplicative)
        , m_bboxOverlayPass(surfaceInfo)
        , m_colorBalancePass(surfaceInfo) { }

void Renderer::render(RenderContext& ctx) {
    glClearColor(0.2f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const SunlightPassInput sunlightPassInput {
        .units = m_unitsForRender,
    };
    const auto sunlightPassOutput = m_sunlightPass.render(ctx, sunlightPassInput);

    const MainPassInput mainPassInput {
        .units = m_unitsForRender,
        .lightSpaceMatrix = sunlightPassOutput.lightSpaceMatrix,
        .shadowDepthTexture = sunlightPassOutput.depthTexture,
        .shadowMix = 0.0f, // TODO
        .query = ctx.query,
    };
    const auto mainPassOutput = m_mainPass.render(ctx, mainPassInput);
//
//    const BrightFilterPassInput brightFilterPassInput {
//        .colorTexture = mainPassOutput.colorTexture,
//        .threshold = m_renderOptions.brightFilterThreshold,
//    };
//    const auto brightFilterPassOutput = m_brightFilterPass.render(ctx, brightFilterPassInput);
//
//    m_gaussianBlurPass.setEnabled(m_renderOptions.enableGaussianBlur);
//    const GaussianBlurPassInput gaussianBlurPassInput {
//        .colorTexture = brightFilterPassOutput.brightColorTexture,
//        .iteration = m_renderOptions.gaussianBlurIteration,
//    };
//    const auto gaussianBlurPassOutput = m_gaussianBlurPass.render(ctx, gaussianBlurPassInput);
//
//    const AdditiveBlendPassInput additiveBlendPassInput {
//        .colorTexture1 = mainPassOutput.colorTexture,
//        .colorTexture2 = gaussianBlurPassOutput.colorTexture,
//    };
//    const auto additivePassOutput = m_additiveBlendPass.render(ctx, additiveBlendPassInput);
//
//    const ToneMapPassInput toneMapPassInput {
//        .colorTexture = additivePassOutput.colorTexture,
//        .exposure = m_renderOptions.toneMapExposure,
//        .gamma = m_renderOptions.toneMapGamma,
//        .enabled = m_renderOptions.toneMapEnabled,
//    };
//    const auto toneMapPassOutput = m_toneMapPass.render(ctx, toneMapPassInput);
//
//    const ColorBalancePassInput colorBalancePassInput {
//        .colorTexture = toneMapPassOutput.colorTexture,
//    };
//    m_colorBalancePass.setColorBalance(m_renderOptions.colorBalance);
//    const auto colorBalancePassOutput = m_colorBalancePass.render(ctx, colorBalancePassInput);

    const OutlinePassInput outlinePassInput {
        .units = m_unitsForRender,
        .depthTexture = mainPassOutput.depthTexture,
        .outlineWidth = m_renderOptions.outlineWidth,
        .outlineDepthThreshold = m_renderOptions.outlineDepthThreshold,
    };
    const auto outlinePassOutput = m_outlinePass.render(ctx, outlinePassInput);

    const AdditiveBlendPassInput multiInput {
        .colorTexture1 = mainPassOutput.colorTexture,
        .colorTexture2 = outlinePassOutput.colorTexture,
    };
    const auto multiOutput = m_outlineMultiplicativeBlendPass.render(ctx, multiInput);

    const BoundingBoxOverlayPassInput bboxOverlayPassInput {
        .units = m_unitsForRender,
        .colorTexture = multiOutput.colorTexture,
        .depthTexture = mainPassOutput.depthTexture,
    };
    m_bboxOverlayPass.render(ctx, bboxOverlayPassInput);

    m_textureRenderer.setSourceTexture(multiOutput.colorTexture, 0);
    m_textureRenderer.render(ctx);
}

void Renderer::resizeResources(const SurfaceInfo& surfaceInfo) {
    m_sunlightPass.resizeResources(surfaceInfo);
    m_mainPass.resizeResources(surfaceInfo);
    m_colorBalancePass.resizeResources(surfaceInfo);
    m_gaussianBlurPass.resizeResources(surfaceInfo);
    m_additiveBlendPass.resizeResources(surfaceInfo);
    m_brightFilterPass.resizeResources(surfaceInfo);
    m_toneMapPass.resizeResources(surfaceInfo);
    m_outlinePass.resizeResources(surfaceInfo);
    m_outlineMultiplicativeBlendPass.resizeResources(surfaceInfo);
    m_bboxOverlayPass.resizeResources(surfaceInfo);
}

RenderOptions& Renderer::renderOptionsMut() {
    return m_renderOptions;
}

void Renderer::syncVisibility(const RuntimeModel& runtimeModel, const RenderModel& renderModel) {
    if (runtimeModel.getObjectVisibilityUpdated()) {
        m_unitsForRender.clear();
        for (const auto& unit: renderModel.units()) {
            if (runtimeModel.getObjectVisibility(unit->objectId())) {
                m_unitsForRender.push_back(unit.get());
            }
        }
    }
}

}
