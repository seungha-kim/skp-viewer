#pragma once
#include "BasePass.h"
#include "ColorTexture.h"
#include "DepthTexture.h"
#include "RenderContext.h"
#include "RenderUnit.h"
#include "SurfaceInfo.h"
#include <memory>

namespace acon {

struct BoundingBoxOverlayPassInput {
    const std::vector<const RenderUnit*>& units;
    const glm::vec3& globalScale;
    const ColorTexture& colorTexture;
    const DepthTexture& depthTexture;
};

struct BoundingBoxOverlayPassOutput { };

class BoundingBoxOverlayPassPimpl;

class BoundingBoxOverlayPass: public BasePass {
public:
    explicit BoundingBoxOverlayPass(const SurfaceInfo& surfaceInfo);
    ~BoundingBoxOverlayPass();
    BoundingBoxOverlayPassOutput render(RenderContext& ctx, const BoundingBoxOverlayPassInput& input);
    void resizeResources(const SurfaceInfo& surfaceInfo) override;

private:
    std::unique_ptr<BoundingBoxOverlayPassPimpl> m_pimpl;
};

}
