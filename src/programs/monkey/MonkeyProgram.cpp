#include "MonkeyProgram.h"
#include <imgui.h>
#include "../../guiCommon.h"

MonkeyProgram::MonkeyProgram(const SurfaceInfo& surfaceInfo)
    : m_sunlightPass(surfaceInfo)
    , m_mainPass(surfaceInfo)
    , m_gaussianBlurPass(surfaceInfo)
    , m_additiveBlendPass(surfaceInfo)
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
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
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

    const AdditiveBlendPassInput additiveBlendPassInput {
        .colorTexture1 = mainPassOutput.colorTexture,
        .colorTexture2 = mainPassOutput.colorTexture,
    };
    const auto additivePassOutput = m_additiveBlendPass.render(ctx, additiveBlendPassInput);

    m_gaussianBlurPass.setEnabled(m_enableGaussianBlur);
    const GaussianBlurPassInput gaussianBlurPassInput {
            .colorTexture = additivePassOutput.colorTexture,
    };
    const auto gaussianBlurPassOutput = m_gaussianBlurPass.render(ctx, gaussianBlurPassInput);

    const ColorBalancePassInput colorBalancePassInput {
            .colorTexture = gaussianBlurPassOutput.colorTexture,
    };
    m_colorBalancePass.setColorBalance(m_colorBalance);
    const auto colorBalancePassOutput = m_colorBalancePass.render(ctx, colorBalancePassInput);

    m_textureRenderer.setSourceTexture(colorBalancePassOutput.colorTexture, 0);
    m_textureRenderer.render(ctx);
}

void MonkeyProgram::processGui(GuiContext &ctx) {
    auto assistant = m_sunlightPass.depthTexture();
    ImGui::Begin("Assistant View", nullptr, windowFlag(ctx));
    ImGui::Image((void*)(intptr_t)assistant, ImVec2(512,512), ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();

    ImGui::Begin("Program Setting", nullptr, windowFlag(ctx));
    ImGui::Checkbox("Gaussian Blur", &m_enableGaussianBlur);
    ImGui::Text("Color Balance");
    ImGui::SliderFloat("Red", &m_colorBalance.r, -1.0f, 1.0f);
    ImGui::SliderFloat("Green", &m_colorBalance.g, -1.0f, 1.0f);
    ImGui::SliderFloat("Blue", &m_colorBalance.b, -1.0f, 1.0f);
    ImGui::End();
}
