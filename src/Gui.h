#pragma once
#include <memory>
#include <GLFW/glfw3.h>
#include "CameraManager.h"
#include "PlaybackState.h"
#include "Window.h"

struct GuiContext {
    CameraManager& cameraManager;
    PlaybackState& playbackState;
    InputController& inputController;
};

class Gui {
public:
    explicit Gui(Window& window);
    ~Gui();
    void process(GuiContext& ctx);
private:
    bool show_demo_window = false;
    bool show_camera_control = false;

    float m_deltas[100] = {0};
    int m_deltasPivot = 0;

    void processRenderInfo(GuiContext& ctx);
    void processCameraControl(GuiContext& ctx) const;
    static float deltasHistogram(void* data, int i);
};
