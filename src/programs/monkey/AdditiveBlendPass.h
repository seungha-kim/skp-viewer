#pragma once
#include <memory>
#include "../../RenderContext.h"
#include "../../graphics/Mesh.h"
#include "../../SurfaceInfo.h"
#include "../../ColorTexture.h"

struct AdditiveBlendPassInput {
    const ColorTexture& colorTexture1;
    const ColorTexture& colorTexture2;
    float c1 = 1.0f;
    float c2 = 1.0f;
};

struct AdditiveBlendPassOutput {
    const ColorTexture& colorTexture;
};

class AdditiveBlendPassPimpl;

class AdditiveBlendPass {
public:
    explicit AdditiveBlendPass(const SurfaceInfo& surfaceInfo);
    ~AdditiveBlendPass();
    AdditiveBlendPassOutput render(RenderContext& ctx, const AdditiveBlendPassInput& input);

private:
    std::unique_ptr<AdditiveBlendPassPimpl> m_pimpl;
};
