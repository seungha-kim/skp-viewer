#include "MonkeyProgram.h"
#include <imgui.h>
#include "../../dev-shell/guiCommon.h"
#include "../checkError.h"

MonkeyProgram::MonkeyProgram(const SurfaceInfo& surfaceInfo)
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

void MonkeyProgram::render(RenderContext &ctx) {
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
        .threshold = m_brightFilterThreshold,
    };
    const auto brightFilterPassOutput = m_brightFilterPass.render(ctx, brightFilterPassInput);

    m_gaussianBlurPass.setEnabled(m_enableGaussianBlur);
    const GaussianBlurPassInput gaussianBlurPassInput {
            .colorTexture = brightFilterPassOutput.brightColorTexture,
            .iteration = m_gaussianBlurIteration,
    };
    const auto gaussianBlurPassOutput = m_gaussianBlurPass.render(ctx, gaussianBlurPassInput);

    const AdditiveBlendPassInput additiveBlendPassInput {
        .colorTexture1 = mainPassOutput.colorTexture,
        .colorTexture2 = gaussianBlurPassOutput.colorTexture,
    };
    const auto additivePassOutput = m_additiveBlendPass.render(ctx, additiveBlendPassInput);

    const ToneMapPassInput toneMapPassInput {
        .colorTexture = additivePassOutput.colorTexture,
        .exposure = m_toneMapExposure,
        .gamma = m_toneMapGamma,
        .enabled = m_toneMapEnabled,
    };
    const auto toneMapPassOutput = m_toneMapPass.render(ctx, toneMapPassInput);

    const ColorBalancePassInput colorBalancePassInput {
            .colorTexture = toneMapPassOutput.colorTexture,
    };
    m_colorBalancePass.setColorBalance(m_colorBalance);
    const auto colorBalancePassOutput = m_colorBalancePass.render(ctx, colorBalancePassInput);

    const OutlinePassInput outlinePassInput {
        .meshes = m_meshes,
        .depthTexture = mainPassOutput.depthTexture,
        .outlineWidth = m_outlineWidth,
        .outlineDepthThreshold = m_outlineDepthThreshold,
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

void MonkeyProgram::processGui(GuiContext &ctx) {
    auto assistant = m_sunlightPass.depthTexture();
    ImGui::Begin("Assistant View", nullptr, windowFlag(ctx));
    ImGui::Image((void*)(intptr_t)assistant, ImVec2(512,512), ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();

    ImGui::Begin("Program Setting", nullptr, windowFlag(ctx));
    ImGui::SliderFloat("Bright Filter Threshold", &m_brightFilterThreshold, 0.0f, 1.0f);
    ImGui::Checkbox("Gaussian Blur", &m_enableGaussianBlur);
    if (m_enableGaussianBlur) {
        ImGui::SliderInt("Iteration", &m_gaussianBlurIteration, 1, 50);
    }
    ImGui::Checkbox("Tone Mapping", &m_toneMapEnabled);
    if (m_toneMapEnabled) {
        ImGui::SliderFloat("Exposure", &m_toneMapExposure, 0.1f, 5.0f);
        ImGui::SliderFloat("Gamma", &m_toneMapGamma, 0.1f, 5.0f);
    }
    ImGui::Text("Color Balance");
    ImGui::SliderFloat("Red", &m_colorBalance.r, 0.1f, 10.0f);
    ImGui::SliderFloat("Green", &m_colorBalance.g, 0.1f, 10.0f);
    ImGui::SliderFloat("Blue", &m_colorBalance.b, 0.1f, 10.0f);

    ImGui::Text("Outline");
    ImGui::SliderFloat("Width", &m_outlineWidth, 0.0f, 10.0f);
    ImGui::SliderFloat("DepthThreshold", &m_outlineDepthThreshold, 0.0f, 100.0f);

    ImGui::End();
}

void MonkeyProgram::resizeResources(const SurfaceInfo &surfaceInfo) {
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
