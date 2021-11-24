#include "SunlightPass.h"
#include "../../Shader.h"

class SunlightPassPimpl {
    friend class SunlightPass;

    GLuint m_colorTexture{};
    GLuint m_depthTexture{};
    GLuint m_fbo{};
    glm::mat4 m_lightSpaceMatrix{};
    std::unique_ptr<Shader> m_subShader;
public:
    ~SunlightPassPimpl() {
        glDeleteTextures(1, &m_colorTexture);
        glDeleteTextures(1, &m_depthTexture);
        glDeleteFramebuffers(1, &m_fbo);
    };
    explicit SunlightPassPimpl(const WindowDimension& dimension)
        : m_subShader(std::make_unique<Shader>("monkey_sub.vert", "monkey_sub.frag")) {
        int fbWidth = dimension.framebufferWidth, fbHeight = dimension.framebufferHeight;

        // http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
        // https://community.khronos.org/t/drawing-depth-texture-bound-to-a-fbo/66919

        // --- Color ---
        glGenTextures(1, &m_colorTexture);
        glBindTexture(GL_TEXTURE_2D, m_colorTexture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fbWidth, fbHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);

        // --- Depth ---
        glGenTextures(1, &m_depthTexture);
        glBindTexture(GL_TEXTURE_2D, m_depthTexture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Remove artefact on the edges of the shadow map
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);

        // TODO: adjustable shadow resolution
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, fbWidth, fbHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
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

    void render(RenderContext &ctx, std::vector<std::unique_ptr<Mesh>>& meshes) {
        auto& cam = ctx.scene.cameraState();

        // solution for peter panning
        // glEnable(GL_CULL_FACE);
        // glFrontFace(GL_CCW);
        // glCullFace(GL_FRONT);

        // TODO: adjustable shadow resolution
        glViewport(0, 0, ctx.windowDimension.framebufferWidth, ctx.windowDimension.framebufferHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

        glClearColor(1.0f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Light point of view
        float width = 30.0f;
        float height = width / (float)ctx.windowDimension.framebufferWidth * (float)ctx.windowDimension.framebufferHeight;
        float top = height / 2.0f;
        float right = width / 2.0f;
        float bottom = -top;
        float left = -right;
        // TODO: proper pos considering view frustum
        auto sunlightPos = glm::vec3(10.0f, 10.0f, 10.0f);
        auto view = glm::lookAt(sunlightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        auto projection = glm::ortho(left, right, bottom, top, 0.1f, 100.0f);
        m_lightSpaceMatrix = projection * view;

        m_subShader->use();
        m_subShader->setMatrix4f("view", view);
        m_subShader->setMatrix4f("projection", projection);
        m_subShader->setVector3f("cameraPos", sunlightPos);
        m_subShader->setVector3f("sunLightDir", ctx.scene.sunLight().direction);
        m_subShader->setVector3f("material.ambient", ctx.globalMaterial.ambient);
        m_subShader->setVector3f("material.diffuse", ctx.globalMaterial.diffuse);
        m_subShader->setVector3f("material.specular", ctx.globalMaterial.specular);
        m_subShader->setFloat("material.shininess", ctx.globalMaterial.shininess);

        for (auto &mesh: meshes) {
            glBindVertexArray(mesh->VAO());

            glm::mat4 model = glm::mat4(1.0f);
            m_subShader->setMatrix4f("model", model);

            glDrawArrays(GL_TRIANGLES, 0, mesh->verticesCount());
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // glDisable(GL_CULL_FACE);
        // glCullFace(GL_BACK);
    }
};

SunlightPass::SunlightPass(const WindowDimension& dimension)
    : m_pimpl(std::make_unique<SunlightPassPimpl>(dimension)) {}

SunlightPass::~SunlightPass() = default;

void SunlightPass::render(RenderContext &ctx, std::vector<std::unique_ptr<Mesh>>& meshes) {
    m_pimpl->render(ctx, meshes);
}

glm::mat4 SunlightPass::lightSpaceMatrix() {
    return m_pimpl->m_lightSpaceMatrix;
}

unsigned SunlightPass::depthTexture() {
    return m_pimpl->m_depthTexture;
}
