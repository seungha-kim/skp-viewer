#pragma once
#include "BasePass.h"
#include "ColorTexture.h"
#include "RenderContext.h"
#include "RenderUnit.h"
#include "SurfaceInfo.h"
#include <memory>

namespace acon {

struct ToneMapPassInput {
    const ColorTexture& colorTexture;
    const float exposure;
    const float gamma;
    bool enabled;
};

struct ToneMapPassOutput {
    const ColorTexture& colorTexture;
};

class ToneMapPassPimpl;

class ToneMapPass: public BasePass {
public:
    explicit ToneMapPass(const SurfaceInfo& surfaceInfo);
    ~ToneMapPass();
    ToneMapPassOutput render(RenderContext& ctx, const ToneMapPassInput& input);
    void resizeResources(const SurfaceInfo& surfaceInfo) override;

private:
    std::unique_ptr<ToneMapPassPimpl> m_pimpl;
};

}
