#pragma once
#include "BasePass.h"
#include "ColorTexture.h"
#include "RenderContext.h"
#include "RenderUnit.h"
#include "SurfaceInfo.h"
#include <memory>

namespace acon {

struct BrightFilterPassInput {
    const ColorTexture& colorTexture;
    const float threshold;
};

struct BrightFilterPassOutput {
    const ColorTexture& brightColorTexture;
};

class BrightFilterPassPimpl;

class BrightFilterPass: public BasePass {
public:
    explicit BrightFilterPass(const SurfaceInfo& surfaceInfo);
    ~BrightFilterPass();
    BrightFilterPassOutput render(RenderContext& ctx, const BrightFilterPassInput& input);
    void resizeResources(const SurfaceInfo& surfaceInfo) override;

private:
    std::unique_ptr<BrightFilterPassPimpl> m_pimpl;
};

}
