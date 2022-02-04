#include "Gui.h"
#include "guiCommon.h"
#include <imgui.h>
#include <optional>

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
    processRenderOptions(ctx);
    processOutliner(ctx);
}

Gui::~Gui() {
    ImGui::DestroyContext();
}

void Gui::processMainMenuBar(GuiContext &ctx) {
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Todo")) {
                printf("TODO\n");
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
        ImGui::Text("Cam Pitch/Yaw: (%.2f, %.2f)", cam.pitch, cam.yaw);
        ImGui::End();
    }
}

float Gui::deltasHistogram(void *data, int i) {
    auto that = (Gui*)data;
    return that->m_deltas[(that->m_deltasPivot + i) % 100];
}

void Gui::processRenderOptions(GuiContext &ctx) {
    auto& renderOptions = ctx.renderOptions;
    auto assistant = 0; // TODO: m_sunlightPass.depthTexture();
    ImGui::Begin("Assistant View", nullptr, windowFlag(ctx));
    ImGui::Image((void*)(intptr_t)assistant, ImVec2(512,512), ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();

    ImGui::Begin("Program Setting", nullptr, windowFlag(ctx));
    ImGui::SliderFloat("Bright Filter Threshold", &renderOptions.brightFilterThreshold, 0.0f, 1.0f);
    ImGui::Checkbox("Gaussian Blur", &renderOptions.enableGaussianBlur);
    if (renderOptions.enableGaussianBlur) {
        ImGui::SliderInt("Iteration", &renderOptions.gaussianBlurIteration, 1, 50);
    }
    ImGui::Checkbox("Tone Mapping", &renderOptions.toneMapEnabled);
    if (renderOptions.toneMapEnabled) {
        ImGui::SliderFloat("Exposure", &renderOptions.toneMapExposure, 0.1f, 5.0f);
        ImGui::SliderFloat("Gamma", &renderOptions.toneMapGamma, 0.1f, 5.0f);
    }
    ImGui::Text("Color Balance");
    ImGui::SliderFloat("Red", &renderOptions.colorBalance.r, 0.1f, 10.0f);
    ImGui::SliderFloat("Green", &renderOptions.colorBalance.g, 0.1f, 10.0f);
    ImGui::SliderFloat("Blue", &renderOptions.colorBalance.b, 0.1f, 10.0f);

    ImGui::Text("Outline");
    ImGui::SliderFloat("Width", &renderOptions.outlineWidth, 0.0f, 10.0f);
    ImGui::SliderFloat("DepthThreshold", &renderOptions.outlineDepthThreshold, 0.0f, 100.0f);

    ImGui::End();
}

void Gui::processOutliner(GuiContext &ctx) {
    static const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
    static const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

    struct Node {
        acon::ObjectId objectId;

        static void displayNode(GuiContext& ctx, Node item) {
            auto childCount = ctx.runtimeModel.getObjectChildrenCount(item.objectId);
            auto name = ctx.runtimeModel.getObjectName(item.objectId);
            auto idString = std::to_string(item.objectId);
            std::string_view label = name.empty() ? idString : name;

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            const bool is_folder = (childCount > 0);
            if (is_folder)
            {
                auto nodeFlag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
                if (ctx.selectedObjectIdOpt == item.objectId) {
                    nodeFlag |= ImGuiTreeNodeFlags_Selected;
                }
                bool open = ImGui::TreeNodeEx(label.data(), nodeFlag);
                if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
                    ctx.selectedObjectIdOpt = item.objectId;
                }
                ImGui::TableNextColumn();
                ImGui::Text("%d", childCount);
                if (open)
                {
                    for (int i = 0; i < childCount; i++) {
                        displayNode(ctx, Node {
                                .objectId = ctx.runtimeModel.getObjectChild(item.objectId, i),
                        });
                    }
                    ImGui::TreePop();
                }
            }
            else
            {
                auto nodeFlag = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth;
                if (ctx.selectedObjectIdOpt == item.objectId) {
                    nodeFlag |= ImGuiTreeNodeFlags_Selected;
                }
                ImGui::TreeNodeEx(label.data(), nodeFlag);
                if (ImGui::IsItemClicked()) {
                    ctx.selectedObjectIdOpt = item.objectId;
                }
                ImGui::TableNextColumn();
                ImGui::Text("%d", 0);
            }
        }
    };

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
    ImGui::Begin("Outliner", nullptr, windowFlag(ctx));
    static ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;

    if (ImGui::BeginTable("Outliner", 3, flags))
    {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
        ImGui::TableSetupColumn("Children", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 12.0f);
        ImGui::TableHeadersRow();

        Node::displayNode(ctx, Node {
            .objectId = acon::ROOT_OBJECT_ID,
        });

        ImGui::EndTable();
    }
    ImGui::End();
    ImGui::PopStyleVar();
}
