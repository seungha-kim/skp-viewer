#pragma once
#include <glad/glad.h>
#include "../../Program.h"
#include <memory>
#include "../../SurfaceInfo.h"
#include "SunlightPass.h"
#include "MainPass.h"
#include "../../TextureRenderer.h"
#include "BloomFilterPass.h"

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
    BloomFilterPass m_bloomFilterPass;
    std::vector<std::unique_ptr<Mesh>> m_meshes;
    TextureRenderer m_textureRenderer;
};
