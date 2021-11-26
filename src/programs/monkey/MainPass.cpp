#include "../../Shader.h"
#include "MainPass.h"
#include "../../OffscreenRenderTarget.h"

class MainPassPimpl {
    friend class MainPass;
    std::unique_ptr<Shader> m_mainShader;
    OffscreenRenderTarget m_offscreenRenderTarget;
    ColorTexture m_colorTexture;
    DepthTexture m_depthTexture;

public:
    explicit MainPassPimpl(const SurfaceInfo& surfaceInfo)
        : m_mainShader(std::make_unique<Shader>("monkey.vert", "monkey.frag"))
        , m_colorTexture(surfaceInfo.physicalWidth, surfaceInfo.physicalHeight)
        , m_depthTexture(surfaceInfo.physicalWidth, surfaceInfo.physicalHeight) {
        m_offscreenRenderTarget.setTargetColorTexture(m_colorTexture);
        m_offscreenRenderTarget.setTargetDepthTexture(m_depthTexture);
    }

    MainPassOutput render(RenderContext &ctx, const MainPassInput& input) {
        m_offscreenRenderTarget.bindAndPrepare(glm::vec3(0.0f, 1.0f, 1.0f), ctx.surfaceInfo.physicalWidth, ctx.surfaceInfo.physicalHeight);

        auto& cam = ctx.scene.cameraState();
        m_mainShader->use();
        m_mainShader->setMatrix4f("view", cam.viewMatrix());
        m_mainShader->setMatrix4f("projection", cam.projectionMatrix());
        m_mainShader->setVector3f("cameraPos", cam.pos);
        m_mainShader->setVector3f("sunLightDir", ctx.scene.sunLight().direction);
        m_mainShader->setVector3f("material.ambient", ctx.globalMaterial.ambient);
        m_mainShader->setVector3f("material.diffuse", ctx.globalMaterial.diffuse);
        m_mainShader->setVector3f("material.specular", ctx.globalMaterial.specular);
        m_mainShader->setFloat("material.shininess", ctx.globalMaterial.shininess);
        m_mainShader->setMatrix4f("lightSpaceMatrix", input.lightSpaceMatrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, input.shadowDepthTexture.textureName());
        m_mainShader->setInt("shadowMap", 0);

        for (auto &mesh: input.meshes) {
            glBindVertexArray(mesh->VAO());

            glm::mat4 model = glm::mat4(1.0f);
            m_mainShader->setMatrix4f("model", model);

            glDrawArrays(GL_TRIANGLES, 0, mesh->verticesCount());
        }

        m_offscreenRenderTarget.unbind();

        return MainPassOutput {
            .colorTexture = m_colorTexture,
        };
    }
};

MainPass::MainPass(const SurfaceInfo &surfaceInfo)
    : m_pimpl(std::make_unique<MainPassPimpl>(surfaceInfo)) {}

MainPass::~MainPass() = default;

MainPassOutput MainPass::render(RenderContext &ctx, const MainPassInput& input) {
    return m_pimpl->render(ctx, input);
}
