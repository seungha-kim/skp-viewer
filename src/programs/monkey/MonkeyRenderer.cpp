#include "MonkeyRenderer.h"
#include "Shader.h"

MonkeyRenderer::MonkeyRenderer(const WindowDimension& dimension)
        : m_mainShader(Shader("monkey.vert", "monkey.frag"))
        , m_framebufferWidth(dimension.framebufferWidth)
        , m_framebufferHeight(dimension.framebufferHeight)
        , m_sunlightPass(SunlightPass(dimension)) {
    initMain();
}

void MonkeyRenderer::render(RenderContext &ctx) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_sunlightPass.render(ctx, m_meshes);
    renderMain(ctx);
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

void MonkeyRenderer::renderMain(RenderContext &ctx) {
    glViewport(0, 0, ctx.windowDimension.framebufferWidth, ctx.windowDimension.framebufferHeight);
    auto& cam = ctx.scene.cameraState();
    m_mainShader.use();
    m_mainShader.setMatrix4f("view", cam.viewMatrix());
    m_mainShader.setMatrix4f("projection", cam.projectionMatrix());
    m_mainShader.setVector3f("cameraPos", cam.pos);
    m_mainShader.setVector3f("sunLightDir", ctx.scene.sunLight().direction);
    m_mainShader.setVector3f("material.ambient", ctx.globalMaterial.ambient);
    m_mainShader.setVector3f("material.diffuse", ctx.globalMaterial.diffuse);
    m_mainShader.setVector3f("material.specular", ctx.globalMaterial.specular);
    m_mainShader.setFloat("material.shininess", ctx.globalMaterial.shininess);
    m_mainShader.setMatrix4f("lightSpaceMatrix", m_sunlightPass.lightSpaceMatrix());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_sunlightPass.depthTexture());
    m_mainShader.setInt("shadowMap", 0);

    for (auto &mesh: m_meshes) {
        glBindVertexArray(mesh->VAO());

        glm::mat4 model = glm::mat4(1.0f);
        m_mainShader.setMatrix4f("model", model);

        glDrawArrays(GL_TRIANGLES, 0, mesh->verticesCount());
    }
}

GLuint MonkeyRenderer::assistantTexture() {
    return m_sunlightPass.depthTexture();
}
