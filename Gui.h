#pragma once
#include <memory>
#include <GLFW/glfw3.h>
#include "CameraOption.h"
#include "RenderOption.h"

struct GuiRenderContext {
    CameraOption& cameraOption;
    RenderOption& renderOption;
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
