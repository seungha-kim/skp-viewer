#pragma once
#include <memory>
#include "../../ColorTexture.h"
#include "../../SurfaceInfo.h"
#include "../../RenderContext.h"
#include "BasePass.h"

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
    void resizeResources(const SurfaceInfo &surfaceInfo) override;
private:
    std::unique_ptr<GaussianBlurPassPimpl> m_pimpl;
};
