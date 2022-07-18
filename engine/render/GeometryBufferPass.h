#pragma once
#include "BasePass.h"
#include "ColorTexture.h"
#include "DepthTexture.h"
#include "RenderContext.h"
#include "RenderUnit.h"
#include "SurfaceInfo.h"
#include <memory>

namespace acon {

struct GeometryBufferPassInput {
    const std::vector<const RenderUnit*>& units;
    const GLuint viewBlockBuffer;
};

struct GeometryBufferPassOutput {
    const ColorTexture& vertexNormalTexture;
    const ColorTexture& faceNormalTexture;
    const DepthTexture& depthTexture;
};

class GeometryBufferPassPimpl;

class GeometryBufferPass: public BasePass {
public:
    explicit GeometryBufferPass(const SurfaceInfo& surfaceInfo);
    ~GeometryBufferPass();
    GeometryBufferPassOutput render(RenderContext& ctx, const GeometryBufferPassInput& input);
    void resizeResources(const SurfaceInfo& surfaceInfo) override;

private:
    std::unique_ptr<GeometryBufferPassPimpl> m_pimpl;
};

}
