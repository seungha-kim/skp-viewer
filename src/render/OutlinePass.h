#pragma once
#include <memory>
#include "../DepthTexture.h"
#include "../ColorTexture.h"
#include "../SurfaceInfo.h"
#include "../RenderContext.h"
#include "../graphics/Mesh.h"
#include "BasePass.h"

struct OutlinePassInput {
    const std::vector<std::unique_ptr<Mesh>>& meshes;
    const DepthTexture& depthTexture;
    const float outlineWidth;
    const float outlineDepthThreshold;
};

struct OutlinePassOutput {
    const ColorTexture& colorTexture;
};

class OutlinePassPimpl;
class OutlinePass: public BasePass {
public:
    OutlinePass(const SurfaceInfo& surfaceInfo);
    ~OutlinePass();
    OutlinePassOutput render(RenderContext& ctx, const OutlinePassInput& input);
    void resizeResources(const SurfaceInfo &surfaceInfo) override;
private:
    std::unique_ptr<OutlinePassPimpl> m_pimpl;
};
