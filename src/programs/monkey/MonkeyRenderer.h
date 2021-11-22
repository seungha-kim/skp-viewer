#pragma once
#include <memory>
#include "BaseRenderer.h"
#include "Shader.h"
#include "graphics/Mesh.h"
#include "../../WindowDimension.h"

class MonkeyRenderer: public BaseRenderer {
public:
    explicit MonkeyRenderer(const WindowDimension& dimension);
    ~MonkeyRenderer() override = default;
    void render(RenderContext& ctx) override;
    GLuint assistantTexture();
private:
    // TODO: update
    int m_framebufferWidth;
    int m_framebufferHeight;
    Shader m_mainShader;
    Shader m_subShader;
    std::vector<std::unique_ptr<Mesh>> m_meshes;
    GLuint m_fbo{};
    GLuint m_colorTexture{};
    GLuint m_depthTexture{};
    glm::mat4 m_lightSpaceMatrix{};

    void initMain();
    void renderMain(RenderContext& ctx);

    void initSub();
    void renderSub(RenderContext& ctx);
};
