#include "SunlightPass.h"
#include "Shader.h"
#include "DepthTexture.h"
#include "ColorTexture.h"
#include "OffscreenRenderTarget.h"

class SunlightPassPimpl {
    friend class SunlightPass;

    std::unique_ptr<ColorTexture> m_colorTexture;
    std::unique_ptr<DepthTexture> m_depthTexture;
    OffscreenRenderTarget m_offscreenRenderTarget;
    glm::mat4 m_lightSpaceMatrix{};
    std::unique_ptr<Shader> m_subShader;
public:
    ~SunlightPassPimpl() = default;
    explicit SunlightPassPimpl(const SurfaceInfo& surfaceInfo)
        : m_subShader(std::make_unique<Shader>("sub.vert", "sub.frag")) {
        resizeResources(surfaceInfo);
    }

    SunlightPassOutput render(RenderContext &ctx, const SunlightPassInput& input) {
        auto& cam = ctx.scene.cameraState();

        // solution for peter panning
        // glEnable(GL_CULL_FACE);
        // glFrontFace(GL_CCW);
        // glCullFace(GL_FRONT);

        // http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
        // https://community.khronos.org/t/drawing-depth-texture-bound-to-a-fbo/66919

        m_offscreenRenderTarget.setTargetDepthTexture(*m_depthTexture);
        m_offscreenRenderTarget.setTargetColorTexture(*m_colorTexture, 0);
        m_offscreenRenderTarget.checkState();

        // TODO: adjustable shadow resolution
        auto binding = m_offscreenRenderTarget.bindAndPrepare(glm::vec3(1.0f, 1.0f, 0.0f), ctx.surfaceInfo.physicalWidth, ctx.surfaceInfo.physicalHeight);

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

            glm::mat4 model = mesh->transform();
            m_subShader->setMatrix4f("model", model);

            glDrawArrays(GL_TRIANGLES, 0, mesh->verticesCount());
        }

        // glDisable(GL_CULL_FACE);
        // glCullFace(GL_BACK);

        return SunlightPassOutput {
            .lightSpaceMatrix = m_lightSpaceMatrix,
            .depthTexture = *m_depthTexture,
        };
    }

    void resizeResources(const SurfaceInfo &surfaceInfo) {
        m_colorTexture = std::make_unique<ColorTexture>(surfaceInfo.physicalWidth, surfaceInfo.physicalHeight);
        m_depthTexture = std::make_unique<DepthTexture>(surfaceInfo.physicalWidth, surfaceInfo.physicalHeight);
    }
};

SunlightPass::SunlightPass(const SurfaceInfo& surfaceInfo)
    : m_pimpl(std::make_unique<SunlightPassPimpl>(surfaceInfo)) {}

SunlightPass::~SunlightPass() = default;

SunlightPassOutput SunlightPass::render(RenderContext &ctx, const SunlightPassInput& input) {
    return m_pimpl->render(ctx, input);
}

GLuint SunlightPass::depthTexture() {
    return m_pimpl->m_depthTexture->textureName();
}

void SunlightPass::resizeResources(const SurfaceInfo &surfaceInfo) {
    m_pimpl->resizeResources(surfaceInfo);
}
