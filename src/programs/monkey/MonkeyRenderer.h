#pragma once

#include "BaseRenderer.h"
#include "Shader.h"
#include "graphics/Mesh.h"

class MonkeyRenderer: public BaseRenderer {
public:
    MonkeyRenderer();
    ~MonkeyRenderer() override = default;
    void render(RenderContext& ctx) override;
private:
    Shader ourShader;
    std::vector<Mesh> m_meshes;
};
