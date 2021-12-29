#pragma once
#include <memory>
#include "../RenderContext.h"
#include "../graphics/Mesh.h"
#include "../SurfaceInfo.h"
#include "../ColorTexture.h"
#include "../DepthTexture.h"
#include "BasePass.h"

struct MainPassInput {
    const std::vector<std::unique_ptr<Mesh>>& meshes;
    const glm::mat4 lightSpaceMatrix;
    const DepthTexture& shadowDepthTexture;
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
    void resizeResources(const SurfaceInfo &surfaceInfo) override;
private:
    std::unique_ptr<MainPassPimpl> m_pimpl;
};
