#include "MonkeyProgram.h"
#include <imgui.h>
#include "../../guiCommon.h"

MonkeyProgram::MonkeyProgram(const SurfaceInfo& surfaceInfo)
    : m_sunlightPass(surfaceInfo)
    , m_mainPass(surfaceInfo)
    , m_bloomFilterPass(surfaceInfo) {
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

    const BloomFilterPassInput bloomFilterPassInput {
        .colorTexture = mainPassOutput.colorTexture
    };
    const auto bloomFilterPassOutput = m_bloomFilterPass.render(ctx, bloomFilterPassInput);

    m_textureRenderer.setTextureToRender(bloomFilterPassOutput.colorTexture);
    m_textureRenderer.render(ctx);
}

void MonkeyProgram::processGui(GuiContext &ctx) {
    auto assistant = m_sunlightPass.depthTexture();
    ImGui::Begin("Assistant View", nullptr, windowFlag(ctx));
    ImGui::Image((void*)(intptr_t)assistant, ImVec2(512,512), ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();
}
