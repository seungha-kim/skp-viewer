#pragma once
#include <memory>
#include <GLFW/glfw3.h>
#include "CameraState.h"
#include "RenderState.h"
#include "InputState.h"

struct GuiRenderContext {
    CameraState& cameraState;
    RenderState& renderState;
    InputState& inputState;
};

class Gui {
public:
    explicit Gui(GLFWwindow& window);
    ~Gui();
    void render(GuiRenderContext& ctx);
private:
    bool show_demo_window = false;
    bool show_camera_control = false;

    void drawRenderInfo(GuiRenderContext& ctx);
    void drawCameraControl(GuiRenderContext& ctx);
};
