#pragma once
#include <memory>
#include "GuiContext.h"

class Gui {
public:
    Gui() = default;
    ~Gui();
    void process(GuiContext& ctx);
private:
    bool show_demo_window = false;
    bool show_camera_control = false;

    float m_deltas[100] = {0};
    int m_deltasPivot = 0;

    void processMainMenuBar(GuiContext& ctx);
    void processRenderInfo(GuiContext& ctx);
    void processRenderOptions(GuiContext &ctx);
    void processSceneControl(GuiContext& ctx) const;
    void processOutliner(GuiContext& ctx);
    static float deltasHistogram(void* data, int i);
};
