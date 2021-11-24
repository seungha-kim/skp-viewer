#include "MonkeyProgram.h"
#include <imgui.h>
#include "../../guiCommon.h"

MonkeyProgram::MonkeyProgram(const WindowDimension& dimension) {
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
    m_renderer = std::make_unique<MonkeyRenderer>(dimension, m_meshes);
}

void MonkeyProgram::render(RenderContext &ctx) {
    m_renderer->render(ctx);
}

void MonkeyProgram::processGui(GuiContext &ctx) {
    auto assistant = m_renderer->assistantTexture();
    ImGui::Begin("Assistant View", nullptr, windowFlag(ctx));
    ImGui::Image((void*)(intptr_t)assistant, ImVec2(512,512), ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();
}
