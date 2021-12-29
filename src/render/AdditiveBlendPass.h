#pragma once
#include <memory>
#include "../RenderContext.h"
#include "../graphics/Mesh.h"
#include "../SurfaceInfo.h"
#include "../ColorTexture.h"
#include "./BasePass.h"

enum class BlendPassKind {
    additive, multiplicative,
};

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

class AdditiveBlendPass: public BasePass {
public:
    explicit AdditiveBlendPass(const SurfaceInfo& surfaceInfo, BlendPassKind kind);
    ~AdditiveBlendPass();
    AdditiveBlendPassOutput render(RenderContext& ctx, const AdditiveBlendPassInput& input);
    void resizeResources(const SurfaceInfo &surfaceInfo) override;

private:
    std::unique_ptr<AdditiveBlendPassPimpl> m_pimpl;
};
