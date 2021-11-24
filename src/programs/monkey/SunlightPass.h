#pragma once
#include <memory>
#include "../../SurfaceInfo.h"
#include "../../graphics/Mesh.h"
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "../../RenderContext.h"

struct SunlightPassInput {
    // TODO: meshes 말고 rootObject 를 받는 것이 적절
    const std::vector<std::unique_ptr<Mesh>>& meshes;
};

struct SunlightPassOutput {
    const glm::mat4 lightSpaceMatrix;
    const GLuint depthTexture;
};

class SunlightPassPimpl;

class SunlightPass {
public:
    explicit SunlightPass(const SurfaceInfo& surfaceInfo);
    ~SunlightPass();
    SunlightPassOutput render(RenderContext& ctx, const SunlightPassInput& input);
    GLuint depthTexture();

private:
    std::unique_ptr<SunlightPassPimpl> m_pimpl;
};
