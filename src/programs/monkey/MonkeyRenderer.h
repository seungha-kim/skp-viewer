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
    Shader ourShader;
    std::vector<std::unique_ptr<Mesh>> m_meshes;
};
