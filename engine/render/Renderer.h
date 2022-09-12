#pragma once
#include "AdditiveBlendPass.h"
#include "BoundingBoxOverlayPass.h"
#include "BrightFilterPass.h"
#include "ColorBalancePass.h"
#include "GaussianBlurPass.h"
#include "GeometryBufferPass.h"
#include "MainPass.h"
#include "OutlinePass.h"
#include "RenderModel.h"
#include "SunlightPass.h"
#include "SurfaceInfo.h"
#include "TextureRenderer.h"
#include "ToneMapPass.h"
#include <functional>
#include <glad/glad.h>
#include <memory>

namespace acon {

enum class DebugViewKind {
    MAIN,
    DEPTH,
    VERTEX_NORMAL,
    FACE_NORMAL,
    OUTLINE,
};

struct RenderOptions {
    glm::vec3 colorBalance {1.0};
    bool enableGaussianBlur {false};

    float toneMapExposure = 2.0f;
    float toneMapGamma = 0.7f;
    bool toneMapEnabled = true;

    int gaussianBlurIteration = 5;
    float brightFilterThreshold = 0.7f;

    int outlineWidth = 1;
    float outlineDepthThreshold = 20.0f;

    bool renderBoundingBox = false;
    DebugViewKind debugViewKind = DebugViewKind::MAIN;
};

class Renderer final {
public:
    explicit Renderer(const SurfaceInfo& surfaceInfo);
    ~Renderer() = default;

    void render(RenderContext& ctx);
    void resizeResources(const SurfaceInfo& surfaceInfo);

    RenderOptions& renderOptionsMut();
    void syncVisibility(const RuntimeModel& runtimeModel, const RenderModel& renderModel);
    void updateViewBlock(RenderContext& ctx) const;

private:
    SunlightPass m_sunlightPass;
    GeometryBufferPass m_geometryBufferPass;
    MainPass m_mainPass;
    ColorBalancePass m_colorBalancePass;
    GaussianBlurPass m_gaussianBlurPass;
    AdditiveBlendPass m_additiveBlendPass;
    BrightFilterPass m_brightFilterPass;
    ToneMapPass m_toneMapPass;
    OutlinePass m_outlinePass;
    AdditiveBlendPass m_outlineMultiplicativeBlendPass;
    TextureRenderer m_textureRenderer;
    RenderOptions m_renderOptions;
    BoundingBoxOverlayPass m_bboxOverlayPass;
    GLuint m_viewBlockBuffer;

    std::vector<const RenderUnit*> m_unitsForRender {};
};

}
