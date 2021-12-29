#include "Renderer.h"
#include "checkError.h"

Renderer::Renderer(const SurfaceInfo& surfaceInfo)
    : m_sunlightPass(surfaceInfo)
    , m_mainPass(surfaceInfo)
    , m_gaussianBlurPass(surfaceInfo)
    , m_additiveBlendPass(surfaceInfo, BlendPassKind::additive)
    , m_brightFilterPass(surfaceInfo)
    , m_toneMapPass(surfaceInfo)
    , m_outlinePass(surfaceInfo)
    , m_outlineMultiplicativeBlendPass(surfaceInfo, BlendPassKind::multiplicative)
    , m_colorBalancePass(surfaceInfo) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile("resources/monkey.obj",
                                             aiProcess_CalcTangentSpace |
                                             aiProcess_Triangulate |
                                             aiProcess_JoinIdenticalVertices |
                                             aiProcess_SortByPType);


    for (int i = 0; i < scene->mNumMeshes; i++) {
        // TODO: transform stack
        m_meshes.push_back(std::make_unique<Mesh>(*scene->mMeshes[i], scene->mRootNode->mTransformation));
    }
}

void Renderer::render(RenderContext &ctx) {
    glClearColor(0.2f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const SunlightPassInput sunlightPassInput {
            .meshes = m_meshes,
    };
    const auto sunlightPassOutput = m_sunlightPass.render(ctx, sunlightPassInput);

    const MainPassInput mainPassInput {
            .meshes = m_meshes,
            .lightSpaceMatrix = sunlightPassOutput.lightSpaceMatrix,
            .shadowDepthTexture = sunlightPassOutput.depthTexture,
    };
    const auto mainPassOutput = m_mainPass.render(ctx, mainPassInput);

    const BrightFilterPassInput brightFilterPassInput {
        .colorTexture = mainPassOutput.colorTexture,
        .threshold = m_renderOptions.brightFilterThreshold,
    };
    const auto brightFilterPassOutput = m_brightFilterPass.render(ctx, brightFilterPassInput);

    m_gaussianBlurPass.setEnabled(m_renderOptions.enableGaussianBlur);
    const GaussianBlurPassInput gaussianBlurPassInput {
            .colorTexture = brightFilterPassOutput.brightColorTexture,
            .iteration = m_renderOptions.gaussianBlurIteration,
    };
    const auto gaussianBlurPassOutput = m_gaussianBlurPass.render(ctx, gaussianBlurPassInput);

    const AdditiveBlendPassInput additiveBlendPassInput {
        .colorTexture1 = mainPassOutput.colorTexture,
        .colorTexture2 = gaussianBlurPassOutput.colorTexture,
    };
    const auto additivePassOutput = m_additiveBlendPass.render(ctx, additiveBlendPassInput);

    const ToneMapPassInput toneMapPassInput {
        .colorTexture = additivePassOutput.colorTexture,
        .exposure = m_renderOptions.toneMapExposure,
        .gamma = m_renderOptions.toneMapGamma,
        .enabled = m_renderOptions.toneMapEnabled,
    };
    const auto toneMapPassOutput = m_toneMapPass.render(ctx, toneMapPassInput);

    const ColorBalancePassInput colorBalancePassInput {
            .colorTexture = toneMapPassOutput.colorTexture,
    };
    m_colorBalancePass.setColorBalance(m_renderOptions.colorBalance);
    const auto colorBalancePassOutput = m_colorBalancePass.render(ctx, colorBalancePassInput);

    const OutlinePassInput outlinePassInput {
        .meshes = m_meshes,
        .depthTexture = mainPassOutput.depthTexture,
        .outlineWidth = m_renderOptions.outlineWidth,
        .outlineDepthThreshold = m_renderOptions.outlineDepthThreshold,
    };
    const auto outlinePassOutput = m_outlinePass.render(ctx, outlinePassInput);

    const AdditiveBlendPassInput multiInput {
        .colorTexture1 = colorBalancePassOutput.colorTexture,
        .colorTexture2 = outlinePassOutput.colorTexture,
    };
    const auto multiOutput = m_outlineMultiplicativeBlendPass.render(ctx, multiInput);

    m_textureRenderer.setSourceTexture(multiOutput.colorTexture, 0);
    m_textureRenderer.render(ctx);
}

void Renderer::resizeResources(const SurfaceInfo &surfaceInfo) {
    m_sunlightPass.resizeResources(surfaceInfo);
    m_mainPass.resizeResources(surfaceInfo);
    m_colorBalancePass.resizeResources(surfaceInfo);
    m_gaussianBlurPass.resizeResources(surfaceInfo);
    m_additiveBlendPass.resizeResources(surfaceInfo);
    m_brightFilterPass.resizeResources(surfaceInfo);
    m_toneMapPass.resizeResources(surfaceInfo);
    m_outlinePass.resizeResources(surfaceInfo);
    m_outlineMultiplicativeBlendPass.resizeResources(surfaceInfo);
}

RenderOptions &Renderer::renderOptionsMut() {
    return m_renderOptions;
}
