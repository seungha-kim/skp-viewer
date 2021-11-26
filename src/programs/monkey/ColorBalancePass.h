#pragma once
#include <memory>
#include "../../RenderContext.h"
#include "../../graphics/Mesh.h"
#include "../../SurfaceInfo.h"
#include "../../ColorTexture.h"

struct ColorBalancePassInput {
    const ColorTexture& colorTexture;
};

struct ColorBalancePassOutput {
    const ColorTexture& colorTexture;
};

class ColorBalancePassPimpl;

class ColorBalancePass {
public:
    explicit ColorBalancePass(const SurfaceInfo& surfaceInfo);
    ~ColorBalancePass();
    ColorBalancePassOutput render(RenderContext& ctx, const ColorBalancePassInput& input);
    void setColorBalance(glm::vec3 colorBalance);
private:
    std::unique_ptr<ColorBalancePassPimpl> m_pimpl;
};
