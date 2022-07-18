#include "GeometryBufferPass.h"
#include "OffscreenRenderTarget.h"
#include "Shader.h"
#include "checkError.h"

namespace acon {

const GLuint VIEW_BLOCK_BINDING_POINT = 0;

class GeometryBufferPassPimpl {
    friend class GeometryBufferPass;
    std::unique_ptr<Shader> m_shader;
    OffscreenRenderTarget m_offscreenRenderTarget;
    std::unique_ptr<ColorTexture> m_vertexNormalTexture;
    std::unique_ptr<ColorTexture> m_faceNormalTexture;
    std::unique_ptr<DepthTexture> m_depthTexture;
    // TODO: alpha testing

public:
    explicit GeometryBufferPassPimpl(const SurfaceInfo& surfaceInfo)
            : m_shader(std::make_unique<Shader>(
                "render.glsl",
                "render.glsl",
                "#define G_BUFFER\n#define VERT\n",
                "#define G_BUFFER\n#define FRAG\n",
                std::vector<UniformBinding> {
                    {"ViewBlock", VIEW_BLOCK_BINDING_POINT},
                })) {
        resizeResources(surfaceInfo);
    }

    GeometryBufferPassOutput render(RenderContext& ctx, const GeometryBufferPassInput& input) {
        glBindBufferBase(GL_UNIFORM_BUFFER, VIEW_BLOCK_BINDING_POINT, input.viewBlockBuffer);

        m_offscreenRenderTarget.setTargetColorTexture(*m_vertexNormalTexture, 0);
        m_offscreenRenderTarget.setTargetColorTexture(*m_faceNormalTexture, 1);
        m_offscreenRenderTarget.setTargetDepthTexture(*m_depthTexture);
        auto binding = m_offscreenRenderTarget.bindAndPrepare(
            glm::vec3(0.0f, 1.0f, 1.0f), ctx.surfaceInfo.physicalWidth, ctx.surfaceInfo.physicalHeight);

        auto& cam = ctx.scene.cameraState();
        m_shader->use();
        // TODO: 추상화
        GLuint buffers[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        glDrawBuffers(2, buffers);
        for (auto& unit: input.units) {
            renderUnit(ctx, input, unit);
        }

        return GeometryBufferPassOutput {
            .vertexNormalTexture = *m_vertexNormalTexture,
            .faceNormalTexture = *m_faceNormalTexture,
            .depthTexture = *m_depthTexture,
        };
    }

    void resizeResources(const SurfaceInfo& surfaceInfo) {
        m_vertexNormalTexture = std::make_unique<ColorTexture>(surfaceInfo.physicalWidth, surfaceInfo.physicalHeight);
        m_faceNormalTexture = std::make_unique<ColorTexture>(surfaceInfo.physicalWidth, surfaceInfo.physicalHeight);
        m_depthTexture = std::make_unique<DepthTexture>(surfaceInfo.physicalWidth, surfaceInfo.physicalHeight);
    }

private:
    void renderUnit(RenderContext& ctx, const GeometryBufferPassInput& input, const RenderUnit* unit) {
        glBindVertexArray(unit->VAO());
        glm::mat4 model = unit->transform();
        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
        m_shader->setMatrix4f("modelMatrix", model);
        m_shader->setMatrix3f("normalMatrix", normalMatrix);
        glDrawArrays(GL_TRIANGLES, 0, unit->verticesCount());
        glActiveTexture(GL_TEXTURE0);
    }
};

GeometryBufferPass::GeometryBufferPass(const SurfaceInfo& surfaceInfo)
        : m_pimpl(std::make_unique<GeometryBufferPassPimpl>(surfaceInfo)) { }

GeometryBufferPass::~GeometryBufferPass() = default;

GeometryBufferPassOutput GeometryBufferPass::render(RenderContext& ctx, const GeometryBufferPassInput& input) {
    return m_pimpl->render(ctx, input);
}

void GeometryBufferPass::resizeResources(const SurfaceInfo& surfaceInfo) {
    m_pimpl->resizeResources(surfaceInfo);
}

}
