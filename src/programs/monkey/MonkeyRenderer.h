#pragma once
#include <memory>
#include "BaseRenderer.h"
#include "Shader.h"
#include "graphics/Mesh.h"

class MonkeyRenderer: public BaseRenderer {
public:
    MonkeyRenderer();
    ~MonkeyRenderer() override = default;
    void render(RenderContext& ctx) override;
private:
    Shader m_mainShader;
    Shader m_subShader;
    Shader m_dispShader;
    std::vector<std::unique_ptr<Mesh>> m_meshes;
    GLuint m_fbo;
    GLuint m_colorTexture;
    GLuint m_depthTexture;
    GLuint m_dispVao;
    GLuint m_dispVbo;

    void initMain();
    void renderMain(RenderContext& ctx);

    void initSub();
    void renderSub(RenderContext& ctx);

    void initSubDisp();
    void renderSubDisp(RenderContext& ctx);
};
