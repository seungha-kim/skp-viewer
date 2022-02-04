#include "Shader.h"
#include "OutlinePass.h"
#include "OffscreenRenderTarget.h"
#include "TextureRenderer.h"
#include "checkError.h"

namespace acon {

class OutlinePassPimpl {
    friend class OutlinePass;

    std::unique_ptr<Shader> m_shader;
    OffscreenRenderTarget m_offscreenRenderTarget;
    std::unique_ptr<ColorTexture> m_colorTexture;
    std::unique_ptr<DepthTexture> m_depthTexture;

public:
    explicit OutlinePassPimpl(const SurfaceInfo& surfaceInfo)
            : m_shader(std::make_unique<Shader>("outline.vert", "outline.frag")) {
        resizeResources(surfaceInfo);
    }

    ~OutlinePassPimpl() = default;

    OutlinePassOutput render(RenderContext &ctx, const OutlinePassInput &input) {
        const auto viewportWidth = ctx.surfaceInfo.physicalWidth;
        const auto viewportHeight = ctx.surfaceInfo.physicalHeight;
        m_offscreenRenderTarget.setTargetColorTexture(*m_colorTexture, 0);
        m_offscreenRenderTarget.setTargetDepthTexture(*m_depthTexture);
        auto binding = m_offscreenRenderTarget.bindAndPrepare(glm::vec3(1.0f, 0.0f, 1.0f), viewportWidth, viewportHeight);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        const auto cam = ctx.scene.cameraState();
        const auto viewMat = cam.viewMatrix();
        const auto viewInv = glm::inverse(viewMat);
        const auto projMat = cam.projectionMatrix();
        const auto projInv = glm::inverse(projMat);
        const auto viewProjMat = projMat * viewMat;
        const auto viewProjInv = glm::inverse(viewProjMat);
        glm::vec4 ViewVecs[2] {glm::vec4(cam.viewVec1(), cam.zNear), glm::vec4(cam.viewVec0(), cam.zFar) * 2.0f};
//        printf("ViewVecs (%f, %f, %f), (%f, %f, %f)\n", ViewVecs[0][0], ViewVecs[0][1], ViewVecs[0][2], ViewVecs[1][0], ViewVecs[1][1], ViewVecs[1][2]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, input.depthTexture.textureName());

        m_shader->use();
        m_shader->setInt("maxzbuffer", 0);
        m_shader->setMatrix4f("ViewProjectionMatrix", viewProjMat);
        m_shader->setMatrix4f("ViewProjectionMatrixInverse", viewProjInv);
        m_shader->setMatrix4f("ViewMatrix", viewMat);
        m_shader->setMatrix4f("ViewMatrixInverse", viewInv);
        m_shader->setMatrix4f("ProjectionMatrix", projMat);
        m_shader->setMatrix4f("ProjectionMatrixInverse", projInv);
        m_shader->setVector4fv("ViewVecs", ViewVecs, 2);
        m_shader->setFloat("width", input.outlineWidth);
        m_shader->setFloat("depthThreshold", input.outlineDepthThreshold);

        for (auto &unit: input.units) {
            glBindVertexArray(unit->VAO());

            glm::mat4 model = unit->transform();
            glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
            m_shader->setMatrix4f("model", model);
            m_shader->setMatrix3f("normalMatrix", normalMatrix);
            m_shader->setMatrix3f("NormalMatrix", normalMatrix);
            m_shader->setMatrix3f("NormalMatrixInverse", glm::inverse(normalMatrix));

            glDrawArrays(GL_TRIANGLES, 0, unit->verticesCount());
        }

        return {
                .colorTexture = *m_colorTexture,
        };

    }

    void resizeResources(const SurfaceInfo &surfaceInfo) {
        m_colorTexture = std::make_unique<ColorTexture>(surfaceInfo.physicalWidth, surfaceInfo.physicalHeight);
        m_depthTexture = std::make_unique<DepthTexture>(surfaceInfo.physicalWidth, surfaceInfo.physicalHeight);
    }
};

OutlinePass::OutlinePass(const SurfaceInfo &surfaceInfo): m_pimpl(std::make_unique<OutlinePassPimpl>(surfaceInfo)) {}

OutlinePass::~OutlinePass() = default;

OutlinePassOutput OutlinePass::render(RenderContext &ctx, const OutlinePassInput &input) {
    return m_pimpl->render(ctx, input);
}

void OutlinePass::resizeResources(const SurfaceInfo &surfaceInfo) {
    m_pimpl->resizeResources(surfaceInfo);
}

}
