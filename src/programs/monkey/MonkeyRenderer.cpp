#include "MonkeyRenderer.h"
#include "Shader.h"

MonkeyRenderer::MonkeyRenderer(const WindowDimension& dimension)
        : m_mainShader(Shader("monkey.vert", "monkey.frag"))
        , m_subShader(Shader("monkey_sub.vert", "monkey_sub.frag"))
        , m_framebufferWidth(dimension.framebufferWidth)
        , m_framebufferHeight(dimension.framebufferHeight) {
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
    // http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
    // https://community.khronos.org/t/drawing-depth-texture-bound-to-a-fbo/66919

    // --- Color ---
    glGenTextures(1, &m_colorTexture);
    glBindTexture(GL_TEXTURE_2D, m_colorTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_framebufferWidth, m_framebufferHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    // --- Depth ---
    glGenTextures(1, &m_depthTexture);
    glBindTexture(GL_TEXTURE_2D, m_depthTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Remove artefact on the edges of the shadow map
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_framebufferWidth, m_framebufferHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

//    glDrawBuffer(GL_NONE);
//    glReadBuffer(GL_NONE);

    glFramebufferTexture(
            GL_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT,
            m_depthTexture,
            0);

    glFramebufferTexture(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            m_colorTexture,
            0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        exit(1);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
