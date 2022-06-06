#pragma once
#include "BasePass.h"
#include "ColorTexture.h"
#include "DepthTexture.h"
#include "RenderContext.h"
#include "RenderUnit.h"
#include "SurfaceInfo.h"
#include <memory>

namespace acon {

struct MainPassInput {
    const std::vector<const RenderUnit*>& units;
    const glm::mat4 lightSpaceMatrix;
    const DepthTexture& shadowDepthTexture;
    const float shadowMix;
    const RenderQuery& query;
    const GLuint viewBlockBuffer;
};

struct MainPassOutput {
    const ColorTexture& colorTexture;
    const DepthTexture& depthTexture;
};

class MainPassPimpl;

class MainPass: public BasePass {
public:
    explicit MainPass(const SurfaceInfo& surfaceInfo);
    ~MainPass();
    MainPassOutput render(RenderContext& ctx, const MainPassInput& input);
    void resizeResources(const SurfaceInfo& surfaceInfo) override;
    void bindViewBlock(GLuint viewBlockBuffer);

private:
    std::unique_ptr<MainPassPimpl> m_pimpl;
};

}
