#pragma once
#include <glad/glad.h>
#include "../../Program.h"
#include <memory>
#include "../../SurfaceInfo.h"
#include "SunlightPass.h"
#include "MainPass.h"
#include "../../TextureRenderer.h"
#include "ColorBalancePass.h"
#include "GaussianBlurPass.h"
#include "AdditiveBlendPass.h"
#include "BrightFilterPass.h"
#include "ToneMapPass.h"
#include "OutlinePass.h"

class MonkeyProgram final: public Program {
public:
    explicit MonkeyProgram(const SurfaceInfo& surfaceInfo);
    ~MonkeyProgram() override = default;

private:
    void render(RenderContext &ctx) override;
    void processGui(GuiContext &ctx) override;
    void resizeResources(const SurfaceInfo &surfaceInfo) override;

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
    std::vector<std::unique_ptr<Mesh>> m_meshes;
    TextureRenderer m_textureRenderer;

    glm::vec3 m_colorBalance{1.0};
    bool m_enableGaussianBlur{false};

    float m_toneMapExposure = 2.0f;
    float m_toneMapGamma = 0.7f;
    bool m_toneMapEnabled = true;

    int m_gaussianBlurIteration = 5;
    float m_brightFilterThreshold = 0.7f;

    float m_outlineWidth = 1.0f;
    float m_outlineDepthThreshold = 20.0f;
};
