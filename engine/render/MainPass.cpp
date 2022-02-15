#include "MainPass.h"
#include "OffscreenRenderTarget.h"
#include "Shader.h"
#include "checkError.h"

namespace acon {

class MainPassPimpl {
    friend class MainPass;
    std::unique_ptr<Shader> m_mainShader;
    OffscreenRenderTarget m_offscreenRenderTarget;
    std::unique_ptr<ColorTexture> m_colorTexture;
    std::unique_ptr<DepthTexture> m_depthTexture;

public:
    explicit MainPassPimpl(const SurfaceInfo& surfaceInfo)
            : m_mainShader(std::make_unique<Shader>("main.vert", "main.frag")) {
        resizeResources(surfaceInfo);
    }

    MainPassOutput render(RenderContext& ctx, const MainPassInput& input) {
        m_offscreenRenderTarget.setTargetColorTexture(*m_colorTexture, 0);
        m_offscreenRenderTarget.setTargetDepthTexture(*m_depthTexture);
        auto binding = m_offscreenRenderTarget.bindAndPrepare(
            glm::vec3(0.0f, 1.0f, 1.0f), ctx.surfaceInfo.physicalWidth, ctx.surfaceInfo.physicalHeight);

        auto& cam = ctx.scene.cameraState();
        m_mainShader->use();
        m_mainShader->setFloat("shadowMix", input.shadowMix);
        m_mainShader->setMatrix4f("view", cam.viewMatrix());
        m_mainShader->setMatrix4f("projection", cam.projectionMatrix());
        m_mainShader->setVector3f("cameraPos", cam.pos);
        m_mainShader->setVector3f("sunLightDir", ctx.scene.sunLight().direction);
        m_mainShader->setMatrix4f("lightSpaceMatrix", input.lightSpaceMatrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, input.shadowDepthTexture.textureName());
        m_mainShader->setInt("shadowMap", 0);

        for (auto& unit: input.units) {
            glBindVertexArray(unit->VAO());
            bool selected = input.query.isSelfOrParentSelected(unit->objectId());
            m_mainShader->setInt("selected", selected);
            glm::mat4 model = unit->transform();
            m_mainShader->setMatrix4f("model", model);

            m_mainShader->setVector3f("frontColor", unit->frontColor());
            m_mainShader->setVector3f("backColor", unit->backColor());
            if (auto frontOpt = unit->frontTextureName()) {
                auto frontTexture = frontOpt.value();
                glActiveTexture(GL_TEXTURE0 + 1);
                glBindTexture(GL_TEXTURE_2D, frontTexture);
                m_mainShader->setInt("frontTexture", 1);
                m_mainShader->setFloat("frontTextureMix", 1.0f);
            } else {
                m_mainShader->setFloat("frontTextureMix", 0.0f);
            };
            if (auto backOpt = unit->backTextureName()) {
                auto backTexture = backOpt.value();
                glActiveTexture(GL_TEXTURE0 + 2);
                glBindTexture(GL_TEXTURE_2D, backTexture);
                m_mainShader->setInt("backTexture", 2);
                m_mainShader->setFloat("backTextureMix", 1.0f);
            } else {
                m_mainShader->setFloat("backTextureMix", 0.0f);
            }

            glDrawArrays(GL_TRIANGLES, 0, unit->verticesCount());
        }
        glActiveTexture(GL_TEXTURE0);

        return MainPassOutput {
            .colorTexture = *m_colorTexture,
            .depthTexture = *m_depthTexture,
        };
    }

    void resizeResources(const SurfaceInfo& surfaceInfo) {
        m_colorTexture = std::make_unique<ColorTexture>(surfaceInfo.physicalWidth, surfaceInfo.physicalHeight);
        m_depthTexture = std::make_unique<DepthTexture>(surfaceInfo.physicalWidth, surfaceInfo.physicalHeight);
    }
};

MainPass::MainPass(const SurfaceInfo& surfaceInfo)
        : m_pimpl(std::make_unique<MainPassPimpl>(surfaceInfo)) { }

MainPass::~MainPass() = default;

MainPassOutput MainPass::render(RenderContext& ctx, const MainPassInput& input) {
    return m_pimpl->render(ctx, input);
}

void MainPass::resizeResources(const SurfaceInfo& surfaceInfo) {
    m_pimpl->resizeResources(surfaceInfo);
}

}
