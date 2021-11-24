#pragma once
#include <memory>
#include "../../graphics/Mesh.h"
#include "../../WindowDimension.h"
#include "../../BaseRenderer.h"

struct MainPassInput {
    const std::vector<std::unique_ptr<Mesh>>& meshes;
    const glm::mat4 lightSpaceMatrix;
    const GLuint depthTexture;
};

struct MainPassOutput {
    const GLuint colorTexture;
};

class MainPassPimpl;

class MainPass {
public:
    explicit MainPass(const WindowDimension& dimension);
    ~MainPass();
    void render(RenderContext& ctx, const MainPassInput& input);
private:
    std::unique_ptr<MainPassPimpl> m_pimpl;
};


