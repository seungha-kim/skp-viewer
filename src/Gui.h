#pragma once
#include <memory>
#include <GLFW/glfw3.h>
#include "CameraState.h"
#include "PlaybackState.h"
#include "InputState.h"
#include "Window.h"

struct GuiRenderContext {
    CameraState& cameraState;
    PlaybackState& playbackState;
    InputState& inputState;
};

class Gui {
public:
    explicit Gui(Window& window);
    ~Gui();
    void render(GuiRenderContext& ctx);
private:
    bool show_demo_window = false;
    bool show_camera_control = false;

    void drawRenderInfo(GuiRenderContext& ctx);
    void drawCameraControl(GuiRenderContext& ctx);
};
