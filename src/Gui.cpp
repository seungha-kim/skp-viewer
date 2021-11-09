#include "Gui.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

ImGuiWindowFlags windowFlag(GuiContext& ctx) {
    ImGuiWindowFlags flag = 0;
    if (ctx.inputController.isCameraRotateMode()) {
        flag |= ImGuiWindowFlags_NoInputs;
    }
    return flag;
}

Gui::Gui(Window& window) {

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window.glfwWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void Gui::process(GuiContext& ctx) {
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

    processRenderInfo(ctx);
    processCameraControl(ctx);
    processMainMenuBar(ctx);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

Gui::~Gui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Gui::processMainMenuBar(GuiContext &ctx) {
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Program"))
        {
            if (ImGui::MenuItem("Test")) {
                ctx.programSelector.changeProgram(ProgramKind::Test);
            }
            if (ImGui::MenuItem("Test2")) {
                ctx.programSelector.changeProgram(ProgramKind::Test2);
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void Gui::processRenderInfo(GuiContext &ctx) {
    float deltaTimeMs = ImGui::GetIO().DeltaTime * 1000.0f;
    m_deltas[m_deltasPivot] = deltaTimeMs;
    ImGui::Begin("Render Info", nullptr, windowFlag(ctx));
    ImGui::Text("%.0f ms", deltaTimeMs);
//    ImGui::Text("WantCaptureMouse: %d", ctx.inputState.isGuiFocused);
    ImGui::PlotHistogram("", deltasHistogram, this, 100, 0, nullptr, 0.0f, 100.0f, ImVec2(0, 20));
    ImGui::Checkbox("Continuous", &ctx.playbackState.forceContinuous);
    ImGui::Checkbox("Manual playback", &ctx.playbackState.manual);
    if (ctx.playbackState.manual) {
        ImGui::SliderFloat("playback", &ctx.playbackState.playback, 0.0f, 10.0f);
    }
    ImGui::Checkbox("Camera Control", &show_camera_control);
    ImGui::Checkbox("Demo Window", &show_demo_window);
    ImGui::End();

    m_deltasPivot = (m_deltasPivot + 1) % 100;
}

void Gui::processCameraControl(GuiContext &ctx) const {
    if (show_camera_control) {
        auto& cm = ctx.cameraManager;
        ImGui::Begin("Camera Control", nullptr, windowFlag(ctx));
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

float Gui::deltasHistogram(void *data, int i) {
    auto that = (Gui*)data;
    return that->m_deltas[(that->m_deltasPivot + i) % 100];
}
