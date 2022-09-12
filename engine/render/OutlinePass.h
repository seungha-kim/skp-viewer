#pragma once
#include "BasePass.h"
#include "ColorTexture.h"
#include "DepthTexture.h"
#include "RenderContext.h"
#include "RenderUnit.h"
#include "SurfaceInfo.h"
#include <memory>

namespace acon {

struct OutlinePassInput {
    const ColorTexture& normalTexture;
    const DepthTexture& depthTexture;
    const GLuint viewBlockBuffer;
    const int lineWidth;
};

struct OutlinePassOutput {
    const ColorTexture& colorTexture;
};

class OutlinePassPimpl;
class OutlinePass: public BasePass {
public:
    explicit OutlinePass(const SurfaceInfo& surfaceInfo);
    ~OutlinePass();
    OutlinePassOutput render(RenderContext& ctx, const OutlinePassInput& input);
    void resizeResources(const SurfaceInfo& surfaceInfo) override;

private:
    std::unique_ptr<OutlinePassPimpl> m_pimpl;
};

}
