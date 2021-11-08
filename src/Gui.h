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

    void processRenderInfo(GuiContext& ctx);
    void processCameraControl(GuiContext& ctx) const;
};
