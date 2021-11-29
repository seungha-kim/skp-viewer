#pragma once
#include <memory>
#include "../../ColorTexture.h"
#include "../../SurfaceInfo.h"
#include "../../RenderContext.h"

struct GaussianBlurPassInput {
    const ColorTexture& colorTexture;
    const int iteration;
};

struct GaussianBlurPassOutput {
    const ColorTexture& colorTexture;
};

class GaussianBlurPassPimpl;
class GaussianBlurPass {
public:
    GaussianBlurPass(const SurfaceInfo& surfaceInfo);
    ~GaussianBlurPass();
    void setEnabled(bool enabled);
    GaussianBlurPassOutput render(RenderContext& ctx, const GaussianBlurPassInput& input);
private:
    std::unique_ptr<GaussianBlurPassPimpl> m_pimpl;
};
