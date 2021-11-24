#pragma once
#include <memory>
#include "BaseRenderer.h"
#include "Shader.h"
#include "graphics/Mesh.h"
#include "../../WindowDimension.h"
#include "SunlightPass.h"

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
    std::vector<std::unique_ptr<Mesh>> m_meshes;
    SunlightPass m_sunlightPass;

    void initMain();
    void renderMain(RenderContext& ctx);
};
