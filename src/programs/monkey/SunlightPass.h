#pragma once
#include <memory>
#include "../../BaseRenderer.h"
#include "../../WindowDimension.h"
#include "../../graphics/Mesh.h"
#include <glm/glm.hpp>
#include <glad/glad.h>

class SunlightPassPimpl;

class SunlightPass {
public:
    explicit SunlightPass(const WindowDimension& dimension);
    ~SunlightPass();
    // TODO: meshes 말고 rootObject 를 받는 것이 적절
    void render(RenderContext& ctx, std::vector<std::unique_ptr<Mesh>>& meshes);
    glm::mat4 lightSpaceMatrix();
    unsigned depthTexture();
private:
    std::unique_ptr<SunlightPassPimpl> m_pimpl;
};
