#pragma once
#include <memory>
#include "../../RenderContext.h"
#include "../../graphics/Mesh.h"
#include "../../SurfaceInfo.h"
#include "../../ColorTexture.h"

struct BloomFilterPassInput {
    const ColorTexture& colorTexture;
};

struct BloomFilterPassOutput {
    const ColorTexture& colorTexture;
};

class BloomFilterPassPimpl;

class BloomFilterPass {
public:
    explicit BloomFilterPass(const SurfaceInfo& surfaceInfo);
    ~BloomFilterPass();
    BloomFilterPassOutput render(RenderContext& ctx, const BloomFilterPassInput& input);
private:
    std::unique_ptr<BloomFilterPassPimpl> m_pimpl;
};
