#pragma once
#include "BasePass.h"
#include "ColorTexture.h"
#include "DepthTexture.h"
#include "RenderContext.h"
#include "RenderUnit.h"
#include "SurfaceInfo.h"
#include <memory>

namespace acon {

struct OutlinePassInput {
    const std::vector<const RenderUnit*>& units;
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
    void resizeResources(const SurfaceInfo& surfaceInfo) override;

private:
    std::unique_ptr<OutlinePassPimpl> m_pimpl;
};

}
