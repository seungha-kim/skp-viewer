#include "../../Shader.h"
#include "MainPass.h"

class MainPassPimpl {
    friend class MainPass;
    std::unique_ptr<Shader> m_mainShader;
public:
    explicit MainPassPimpl(const WindowDimension& dimension)
        : m_mainShader(std::make_unique<Shader>("monkey.vert", "monkey.frag")) {
    }

    MainPassOutput render(RenderContext &ctx, const MainPassInput& input) {
        glViewport(0, 0, ctx.windowDimension.framebufferWidth, ctx.windowDimension.framebufferHeight);
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
        glBindTexture(GL_TEXTURE_2D, input.depthTexture);
        m_mainShader->setInt("shadowMap", 0);

        for (auto &mesh: input.meshes) {
            glBindVertexArray(mesh->VAO());

            glm::mat4 model = glm::mat4(1.0f);
            m_mainShader->setMatrix4f("model", model);

            glDrawArrays(GL_TRIANGLES, 0, mesh->verticesCount());
        }

        return MainPassOutput {
            .colorTexture = 0, // TODO
        };
    }
};

MainPass::MainPass(const WindowDimension &dimension)
    : m_pimpl(std::make_unique<MainPassPimpl>(dimension)) {}

MainPass::~MainPass() = default;

MainPassOutput MainPass::render(RenderContext &ctx, const MainPassInput& input) {
    return m_pimpl->render(ctx, input);
}
