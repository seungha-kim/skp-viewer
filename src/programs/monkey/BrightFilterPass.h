#pragma once
#include <memory>
#include "../../RenderContext.h"
#include "../../graphics/Mesh.h"
#include "../../SurfaceInfo.h"
#include "../../ColorTexture.h"

struct BrightFilterPassInput {
    const ColorTexture& colorTexture;
    const float threshold;
};

struct BrightFilterPassOutput {
    const ColorTexture& brightColorTexture;
};

class BrightFilterPassPimpl;

class BrightFilterPass {
public:
    explicit BrightFilterPass(const SurfaceInfo& surfaceInfo);
    ~BrightFilterPass();
    BrightFilterPassOutput render(RenderContext& ctx, const BrightFilterPassInput& input);

private:
    std::unique_ptr<BrightFilterPassPimpl> m_pimpl;
};