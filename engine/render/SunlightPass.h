#pragma once
#include "BasePass.h"
#include "DepthTexture.h"
#include "RenderContext.h"
#include "RenderUnit.h"
#include "SurfaceInfo.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>

namespace acon {

struct SunlightPassInput {
    // TODO: units 말고 rootObject 를 받는 것이 적절.
    const std::vector<const RenderUnit*>& units;
};

struct SunlightPassOutput {
    const glm::mat4 lightSpaceMatrix;
    const DepthTexture& depthTexture;
};

class SunlightPassPimpl;

class SunlightPass: public BasePass {
public:
    explicit SunlightPass(const SurfaceInfo& surfaceInfo);
    ~SunlightPass();
    SunlightPassOutput render(RenderContext& ctx, const SunlightPassInput& input);
    void resizeResources(const SurfaceInfo& surfaceInfo) override;
    GLuint depthTexture();

private:
    std::unique_ptr<SunlightPassPimpl> m_pimpl;
};

}
