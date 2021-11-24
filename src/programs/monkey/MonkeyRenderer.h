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
    MonkeyRenderer(const WindowDimension& dimension, const std::vector<std::unique_ptr<Mesh>>& meshes);
    ~MonkeyRenderer() override = default;
    void render(RenderContext& ctx) override;
    GLuint assistantTexture();
private:
    SunlightPass m_sunlightPass;
    MainPass m_mainPass;
    const std::vector<std::unique_ptr<Mesh>>& m_meshes;
};
