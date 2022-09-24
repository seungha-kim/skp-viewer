#pragma once
#include "./BasePass.h"
#include "ColorTexture.h"
#include "RenderContext.h"
#include "RenderUnit.h"
#include "SurfaceInfo.h"
#include <memory>

namespace acon {

enum class BlendPassKind {
    additive,
    multiplicative,
};

struct BlendPassInput {
    const ColorTexture& colorTexture1;
    const ColorTexture& colorTexture2;
    float c1 = 1.0f;
    float c2 = 1.0f;
};

struct BlendPassOutput {
    const ColorTexture& colorTexture;
};

class BlendPassPimpl;

class BlendPass: public BasePass {
public:
    explicit BlendPass(const SurfaceInfo& surfaceInfo, BlendPassKind kind);
    ~BlendPass();
    BlendPassOutput render(RenderContext& ctx, const BlendPassInput& input);
    void resizeResources(const SurfaceInfo& surfaceInfo) override;

private:
    std::unique_ptr<BlendPassPimpl> m_pimpl;
};

}
