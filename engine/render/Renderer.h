#pragma once
#include <glad/glad.h>
#include <memory>
#include "SurfaceInfo.h"
#include "SunlightPass.h"
#include "MainPass.h"
#include "TextureRenderer.h"
#include "ColorBalancePass.h"
#include "GaussianBlurPass.h"
#include "AdditiveBlendPass.h"
#include "BrightFilterPass.h"
#include "ToneMapPass.h"
#include "OutlinePass.h"

struct RenderOptions {
    glm::vec3 colorBalance{1.0};
    bool enableGaussianBlur{false};

    float toneMapExposure = 2.0f;
    float toneMapGamma = 0.7f;
    bool toneMapEnabled = true;

    int gaussianBlurIteration = 5;
    float brightFilterThreshold = 0.7f;

    float outlineWidth = 1.0f;
    float outlineDepthThreshold = 20.0f;
};

class Renderer final {
public:
    explicit Renderer(const SurfaceInfo& surfaceInfo, const AbstractReader& model);
    ~Renderer() = default;

    void render(RenderContext &ctx);
    void resizeResources(const SurfaceInfo &surfaceInfo);

    RenderOptions& renderOptionsMut();

private:
    SunlightPass m_sunlightPass;
    MainPass m_mainPass;
    ColorBalancePass m_colorBalancePass;
    GaussianBlurPass m_gaussianBlurPass;
    AdditiveBlendPass m_additiveBlendPass;
    BrightFilterPass m_brightFilterPass;
    ToneMapPass m_toneMapPass;
    OutlinePass m_outlinePass;
    AdditiveBlendPass m_outlineMultiplicativeBlendPass;
    std::vector<std::unique_ptr<RenderMesh>> m_meshes;
    TextureRenderer m_textureRenderer;
    RenderOptions m_renderOptions;
};
