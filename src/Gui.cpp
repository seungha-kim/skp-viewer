#include "Gui.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


Gui::Gui(Window& window) {

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window.glfwWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void Gui::render(GuiRenderContext& ctx) {
    auto& io = ImGui::GetIO();
    if (io.WantCaptureKeyboard || io.WantCaptureMouse) {
        ctx.inputController.setGuiFocused(true);
    } else {
        ctx.inputController.setGuiFocused(false);
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    drawRenderInfo(ctx);
    drawCameraControl(ctx);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

Gui::~Gui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Gui::drawRenderInfo(GuiRenderContext &ctx) {
    static float deltas[100];
    static int pivot = 0;
    static bool init = false;
    static float (*histogram)(void*, int) = [](void*, int i) {
        return deltas[(pivot + i) % 100];
    };
    if (!init) {
        for (float& delta : deltas) {
            delta = 0.0f;
        }
        init = true;
    }

    float deltaTimeMs = ImGui::GetIO().DeltaTime * 1000.0f;
    deltas[pivot] = deltaTimeMs;
    ImGui::Begin("Render Info");
    ImGui::Text("%.0f ms", deltaTimeMs);
//    ImGui::Text("WantCaptureMouse: %d", ctx.inputState.isGuiFocused);
    ImGui::PlotHistogram("", histogram, nullptr, 100, 0, nullptr, 0.0f, 100.0f, ImVec2(0, 20));
    ImGui::Checkbox("Continuous", &ctx.playbackState.forceContinuous);
    ImGui::Checkbox("Manual playback", &ctx.playbackState.manual);
    if (ctx.playbackState.manual) {
        ImGui::SliderFloat("playback", &ctx.playbackState.playback, 0.0f, 10.0f);
    }
    ImGui::Checkbox("Camera Control", &show_camera_control);
    ImGui::Checkbox("Demo Window", &show_demo_window);
    ImGui::End();

    pivot = (pivot + 1) % 100;
}

void Gui::drawCameraControl(GuiRenderContext &ctx) {
    if (show_camera_control) {
        auto& cm = ctx.cameraManager;
        ImGui::Begin("Camera Control");
        if (ImGui::Button("Add Camera")) {
            cm.addCamera();
        }
        ImGui::SameLine();
        if (ImGui::Button("Remove Camera")) {
            cm.removeActiveCamera();
        }
        const auto& cs = cm.cameras();
        for (int i = 0; i < cs.size(); i++) {
            char buf[32];
            sprintf(buf, "Camera %d", i);
            if (ImGui::Selectable(buf, i == cm.activeCameraIndex())) {
                cm.setActiveCamera(i);
            }
        }
        auto& cam = cm.activeCameraMut();
        ImGui::SliderFloat("fovy", &cam.fovyDeg, 10.0f, 170.0f);
        ImGui::SliderFloat("zNear", &cam.zNear, 0.01f, 10.0f);
        ImGui::SliderFloat("zFar", &cam.zFar, 0.1f, 200.0f);
        ImGui::End();
    }
}
