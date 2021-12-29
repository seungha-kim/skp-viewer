#include "Gui.h"
#include "guiCommon.h"
#include <imgui.h>

void Gui::process(GuiContext& ctx) {
    auto& io = ImGui::GetIO();
    if (io.WantCaptureKeyboard || io.WantCaptureMouse) {
        ctx.inputController.setGuiFocused(true);
    } else {
        ctx.inputController.setGuiFocused(false);
    }

    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    processRenderInfo(ctx);
    processSceneControl(ctx);
    processMainMenuBar(ctx);
}

Gui::~Gui() {
    ImGui::DestroyContext();
}

void Gui::processMainMenuBar(GuiContext &ctx) {
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Program"))
        {
            if (ImGui::MenuItem("Test")) {
                ctx.programSelector.changeProgram(ctx.surfaceInfo, ProgramKind::Test);
            }
            if (ImGui::MenuItem("Test2")) {
                ctx.programSelector.changeProgram(ctx.surfaceInfo, ProgramKind::Test2);
            }
            if (ImGui::MenuItem("Monkey")) {
                ctx.programSelector.changeProgram(ctx.surfaceInfo, ProgramKind::Monkey);
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
    auto& dim = ctx.surfaceInfo;
    ImGui::Text("window size: %d * %d", dim.logicalWidth, dim.logicalHeight);
    ImGui::Text("framebuffer size: %d * %d", dim.physicalWidth, dim.physicalHeight);
    ImGui::Text("pixel scale: %.0f * %.0f", dim.contentScaleX, dim.contentScaleY);
    ImGui::Text("render time: %.0f ms", deltaTimeMs);
//    ImGui::Text("WantCaptureMouse: %d", ctx.inputState.isGuiFocused);
    ImGui::PlotHistogram("", deltasHistogram, this, 100, 0, nullptr, 0.0f, 100.0f, ImVec2(0, 20));
    ImGui::Checkbox("Continuous", &ctx.playbackState.forceContinuous);
    ImGui::Checkbox("Manual playback", &ctx.playbackState.manual);
    if (ctx.playbackState.manual) {
        ImGui::SliderFloat("playback", &ctx.playbackState.playback, 0.0f, 10.0f);
    }
    ImGui::Checkbox("Scenes", &show_camera_control);
    ImGui::Checkbox("Demo Window", &show_demo_window);

    if (ImGui::TreeNode("Global Material")) {
        ImGui::ColorEdit3("ambient", (float*)&ctx.globalMaterial.ambient);
        ImGui::ColorEdit3("diffuse", (float*)&ctx.globalMaterial.diffuse);
        ImGui::ColorEdit3("specular", (float*)&ctx.globalMaterial.specular);
        ImGui::SliderFloat("shininess", &ctx.globalMaterial.shininess, 1.0f, 1024.0f);
        ImGui::TreePop();
    }

    ImGui::End();

    m_deltasPivot = (m_deltasPivot + 1) % 100;
}

void Gui::processSceneControl(GuiContext &ctx) const {
    if (show_camera_control) {
        auto& cm = ctx.sceneManager;
        ImGui::Begin("Scenes", nullptr, windowFlag(ctx));
        if (ImGui::Button("Add Scene")) {
            cm.addScene();
        }
        ImGui::SameLine();
        if (ImGui::Button("Remove Scene")) {
            cm.removeActiveScene();
        }
        const auto& cs = cm.scenes();
        for (int i = 0; i < cs.size(); i++) {
            char buf[32];
            sprintf(buf, "Scene %d", i);
            if (ImGui::Selectable(buf, i == cm.activeSceneIndex())) {
                cm.setActiveScene(i);
            }
        }
        auto& scene = cm.activeSceneMut();
        auto& cam = scene.cameraStateMut();
        auto& sunLight = scene.sunLightMut();

        ImGui::SliderFloat("lightDirX", &sunLight.direction.x, -1.0f, 1.0f);
        ImGui::SliderFloat("lightDirY", &sunLight.direction.y, -1.0f, 1.0f);
        ImGui::SliderFloat("lightDirZ", &sunLight.direction.z, -1.0f, 1.0f);
        ImGui::SliderFloat("fovy", &cam.fovyDeg, 10.0f, 170.0f);
        ImGui::SliderFloat("zNear", &cam.zNear, 0.01f, 10.0f);
        ImGui::SliderFloat("zFar", &cam.zFar, 0.1f, 200.0f);
        ImGui::Text("Cam Pos: (%.2f, %.2f, %.2f)", cam.pos.x, cam.pos.y, cam.pos.z);
        ImGui::Text("Cam Up: (%.2f, %.2f, %.2f)", cam.up.x, cam.up.y, cam.up.z);
        ImGui::Text("Cam Pitch/Yaw: (%.2f, %.2f)", cam.yaw, cam.pitch);
        ImGui::End();
    }
}

float Gui::deltasHistogram(void *data, int i) {
    auto that = (Gui*)data;
    return that->m_deltas[(that->m_deltasPivot + i) % 100];
}
