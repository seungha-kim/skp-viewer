#include "MonkeyRenderer.h"
#include "Shader.h"

MonkeyRenderer::MonkeyRenderer()
        : m_mainShader(Shader("monkey.vert", "monkey.frag"))
        , m_subShader(Shader("monkey_sub.vert", "monkey_sub.frag")) {
    initMain();
    initSub();
}

void MonkeyRenderer::render(RenderContext &ctx) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderMain(ctx);
    renderSub(ctx);
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

    for (auto &mesh: m_meshes) {
        glBindVertexArray(mesh->VAO());

        glm::mat4 model = glm::mat4(1.0f);
        m_mainShader.setMatrix4f("model", model);

        glDrawArrays(GL_TRIANGLES, 0, mesh->verticesCount());
    }
}

void MonkeyRenderer::initSub() {

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glGenTextures(1, &m_colorTexture);
    glBindTexture(GL_TEXTURE_2D, m_colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, 1024, 768, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glGenRenderbuffers(1, &m_depthTexture);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthTexture);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 768);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthTexture);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_colorTexture, 0);
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        exit(1);
    }
}

void MonkeyRenderer::renderSub(RenderContext &ctx) {
    auto& cam = ctx.scene.cameraState();
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glClearColor(1.0f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_subShader.use();
    m_subShader.setMatrix4f("view", cam.viewMatrix());
    m_subShader.setMatrix4f("projection", cam.projectionMatrix());
    m_subShader.setVector3f("cameraPos", cam.pos);
    m_subShader.setVector3f("sunLightDir", ctx.scene.sunLight().direction);
    m_subShader.setVector3f("material.ambient", ctx.globalMaterial.ambient);
    m_subShader.setVector3f("material.diffuse", ctx.globalMaterial.diffuse);
    m_subShader.setVector3f("material.specular", ctx.globalMaterial.specular);
    m_subShader.setFloat("material.shininess", ctx.globalMaterial.shininess);

    for (auto &mesh: m_meshes) {
        glBindVertexArray(mesh->VAO());

        glm::mat4 model = glm::mat4(1.0f);
        m_mainShader.setMatrix4f("model", model);

        glDrawArrays(GL_TRIANGLES, 0, mesh->verticesCount());
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

GLuint MonkeyRenderer::assistantTexture() {
    return m_colorTexture;
}
