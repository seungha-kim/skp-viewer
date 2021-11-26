#include "SunlightPass.h"
#include "../../Shader.h"
#include "../../DepthTexture.h"

class SunlightPassPimpl {
    friend class SunlightPass;

    GLuint m_colorTexture{};
    DepthTexture m_depthTexture;
    GLuint m_fbo{};
    glm::mat4 m_lightSpaceMatrix{};
    std::unique_ptr<Shader> m_subShader;
public:
    ~SunlightPassPimpl() {
        glDeleteTextures(1, &m_colorTexture);
        glDeleteFramebuffers(1, &m_fbo);
    };
    explicit SunlightPassPimpl(const SurfaceInfo& surfaceInfo)
        : m_subShader(std::make_unique<Shader>("monkey_sub.vert", "monkey_sub.frag"))
        , m_depthTexture(surfaceInfo.physicalWidth, surfaceInfo.physicalHeight) {
        int fbWidth = surfaceInfo.physicalWidth, fbHeight = surfaceInfo.physicalHeight;

        // http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
        // https://community.khronos.org/t/drawing-depth-texture-bound-to-a-fbo/66919

        // --- Color ---
        glGenTextures(1, &m_colorTexture);
        glBindTexture(GL_TEXTURE_2D, m_colorTexture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fbWidth, fbHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);

        glGenFramebuffers(1, &m_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

//    glDrawBuffer(GL_NONE);
//    glReadBuffer(GL_NONE);

        glFramebufferTexture(
                GL_FRAMEBUFFER,
                GL_DEPTH_ATTACHMENT,
                m_depthTexture.textureName(),
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

    SunlightPassOutput render(RenderContext &ctx, const SunlightPassInput& input) {
        auto& cam = ctx.scene.cameraState();

        // solution for peter panning
        // glEnable(GL_CULL_FACE);
        // glFrontFace(GL_CCW);
        // glCullFace(GL_FRONT);

        // TODO: adjustable shadow resolution
        glViewport(0, 0, ctx.surfaceInfo.physicalWidth, ctx.surfaceInfo.physicalHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

        glClearColor(1.0f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Light point of view
        float width = 30.0f;
        float height = width / (float)ctx.surfaceInfo.physicalWidth * (float)ctx.surfaceInfo.physicalHeight;
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

        for (auto &mesh: input.meshes) {
            glBindVertexArray(mesh->VAO());

            glm::mat4 model = glm::mat4(1.0f);
            m_subShader->setMatrix4f("model", model);

            glDrawArrays(GL_TRIANGLES, 0, mesh->verticesCount());
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // glDisable(GL_CULL_FACE);
        // glCullFace(GL_BACK);

        return SunlightPassOutput {
            .lightSpaceMatrix = m_lightSpaceMatrix,
            .depthTexture = m_depthTexture.textureName(),
        };
    }
};

SunlightPass::SunlightPass(const SurfaceInfo& surfaceInfo)
    : m_pimpl(std::make_unique<SunlightPassPimpl>(surfaceInfo)) {}

SunlightPass::~SunlightPass() = default;

SunlightPassOutput SunlightPass::render(RenderContext &ctx, const SunlightPassInput& input) {
    return m_pimpl->render(ctx, input);
}

GLuint SunlightPass::depthTexture() {
    return m_pimpl->m_depthTexture.textureName();
}
