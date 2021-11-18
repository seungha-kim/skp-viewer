#pragma once
#include <memory>
#include <GLFW/glfw3.h>
#include "Window.h"
#include "GuiContext.h"

class Gui {
public:
    explicit Gui(Window& window);
    ~Gui();
    void begin();
    void process(GuiContext& ctx);
    void end();
private:
    bool show_demo_window = false;
    bool show_camera_control = false;

    float m_deltas[100] = {0};
    int m_deltasPivot = 0;

    void processMainMenuBar(GuiContext& ctx);
    void processRenderInfo(GuiContext& ctx);
    void processSceneControl(GuiContext& ctx) const;
    static float deltasHistogram(void* data, int i);
};
