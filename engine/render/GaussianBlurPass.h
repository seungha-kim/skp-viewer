#pragma once
#include "BasePass.h"
#include "ColorTexture.h"
#include "RenderContext.h"
#include "SurfaceInfo.h"
#include <memory>

namespace acon {

struct GaussianBlurPassInput {
    const ColorTexture& colorTexture;
    const int iteration;
};

struct GaussianBlurPassOutput {
    const ColorTexture& colorTexture;
};

class GaussianBlurPassPimpl;
class GaussianBlurPass: BasePass {
public:
    GaussianBlurPass(const SurfaceInfo& surfaceInfo);
    ~GaussianBlurPass();
    void setEnabled(bool enabled);
    GaussianBlurPassOutput render(RenderContext& ctx, const GaussianBlurPassInput& input);
    void resizeResources(const SurfaceInfo& surfaceInfo) override;

private:
    std::unique_ptr<GaussianBlurPassPimpl> m_pimpl;
};

}
