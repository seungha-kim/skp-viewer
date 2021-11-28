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

class MonkeyProgram final: public Program {
public:
    explicit MonkeyProgram(const SurfaceInfo& surfaceInfo);
    ~MonkeyProgram() override = default;

private:
    void render(RenderContext &ctx) override;
    void processGui(GuiContext &ctx) override;

private:
    SunlightPass m_sunlightPass;
    MainPass m_mainPass;
    ColorBalancePass m_colorBalancePass;
    GaussianBlurPass m_gaussianBlurPass;
    AdditiveBlendPass m_additiveBlendPass;
    BrightFilterPass m_brightFilterPass;
    std::vector<std::unique_ptr<Mesh>> m_meshes;
    TextureRenderer m_textureRenderer;

    glm::vec3 m_colorBalance{};
    bool m_enableGaussianBlur{true};
};
