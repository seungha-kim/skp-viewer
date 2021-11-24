#include "MonkeyRenderer.h"
#include "Shader.h"

MonkeyRenderer::MonkeyRenderer(const WindowDimension& dimension)
        : m_mainPass{dimension}
        , m_sunlightPass{dimension} {
    initMain();
}

void MonkeyRenderer::render(RenderContext &ctx) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_sunlightPass.render(ctx, m_meshes);

    const MainPassInput mainPassInput {
        .meshes = m_meshes,
        .lightSpaceMatrix = m_sunlightPass.lightSpaceMatrix(),
        .depthTexture = m_sunlightPass.depthTexture(),
    };
    m_mainPass.render(ctx, mainPassInput);
}

void MonkeyRenderer::initMain() {
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

GLuint MonkeyRenderer::assistantTexture() {
    return m_sunlightPass.depthTexture();
}
