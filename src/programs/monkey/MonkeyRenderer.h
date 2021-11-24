#pragma once
#include <memory>
#include "BaseRenderer.h"
#include "Shader.h"
#include "graphics/Mesh.h"
#include "../../WindowDimension.h"
#include "SunlightPass.h"
#include "MainPass.h"

class MonkeyRenderer: public BaseRenderer {
public:
    explicit MonkeyRenderer(const WindowDimension& dimension);
    ~MonkeyRenderer() override = default;
    void render(RenderContext& ctx) override;
    GLuint assistantTexture();
private:
    std::vector<std::unique_ptr<Mesh>> m_meshes;

    SunlightPass m_sunlightPass;
    MainPass m_mainPass;

    void initMain();
};
