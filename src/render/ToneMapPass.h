#pragma once
#include <memory>
#include "../RenderContext.h"
#include "../graphics/Mesh.h"
#include "../SurfaceInfo.h"
#include "../ColorTexture.h"
#include "BasePass.h"

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
    void resizeResources(const SurfaceInfo &surfaceInfo) override;
private:
    std::unique_ptr<ToneMapPassPimpl> m_pimpl;
};
