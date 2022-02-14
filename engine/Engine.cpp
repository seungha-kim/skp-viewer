#include "Engine.h"
#include "render/checkError.h"
#include <glad/glad.h>
#include <glm/gtc/random.hpp>
#include "render/RenderModel.h"
#include "runtime/RuntimeModel.h"

namespace acon {

class RenderQueryImpl: public RenderQuery {
public:
    explicit RenderQueryImpl(const RuntimeModel& runtimeModel): m_runtimeModel(runtimeModel) {}
    ~RenderQueryImpl() override = default;

    [[nodiscard]] bool isSelfOrParentSelected(ObjectId objectId) const override {
        // TODO: 이게 매 render 매 unit 마다 호출되고 있는데, 미리 계산해서 더 효율적으로 개선할 수 있을듯.
        auto selectedObjectIdOpt = m_runtimeModel.selectedObjectIdOpt();
        if (!selectedObjectIdOpt) return false;
        auto selectedObjectId = selectedObjectIdOpt.value();
        std::optional<ObjectId> currentId = objectId;
        while (currentId) {
            if (currentId.value() == selectedObjectId) {
                return true;
            }
            currentId = m_runtimeModel.getObjectParent(currentId.value());
        }
        return false;
    }
private:
    const RuntimeModel& m_runtimeModel;
};

Engine::Engine(const AbstractReader& reader) {
    auto [runtimeModel, renderModel] = buildModel(reader);
    m_runtimeModel = std::move(runtimeModel);
    m_renderModel = std::move(renderModel);
}

Engine::~Engine() = default;

const SceneManager &Engine::sceneManager() const {
    return m_sceneManager;
}

SceneManager &Engine::sceneManagerMut() {
    return m_sceneManager;
}

const SurfaceInfo &Engine::surfaceInfo() const {
    return m_surfaceInfo;
}

void Engine::prepareToRender(const SurfaceInfo& surfaceInfo) {
    if (m_preparedToRender) return;
    m_surfaceInfo = surfaceInfo;
    m_sceneManager.updateAspectRatio(surfaceInfo);
    m_renderModel->prepareToRender();
    m_renderer = std::make_unique<Renderer>(surfaceInfo);
    m_preparedToRender = true;
    glEnable(GL_DEPTH_TEST);
}

void Engine::render(float playbackValue) {
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("WARN - NOT COMPLETE\n");
        return;
    }
    updateTextures();
//    printf("RENDER %f\n", playbackValue);
    m_renderer->syncVisibility(*m_runtimeModel, *m_renderModel);
    auto query = RenderQueryImpl(*m_runtimeModel);
    RenderContext renderCtx {
            .scene = sceneManager().activeScene(),
            .playbackValue = playbackValue,
            .surfaceInfo = m_surfaceInfo,
            .renderModel = *m_renderModel,
            .query = query,
    };
    m_renderer->render(renderCtx);
    m_runtimeModel->clearFrameFlags();
}

void Engine::resize(const SurfaceInfo &surfaceInfo) {
    m_surfaceInfo = surfaceInfo;
    m_sizeUpdated = true;
}

void Engine::updateTextures() {
    if (!m_sizeUpdated) return;
    m_sizeUpdated = false;

    auto& cam = m_sceneManager.activeSceneMut().cameraStateMut();
    cam.aspectWidth = (float)m_surfaceInfo.logicalWidth;
    cam.aspectHeight = (float)m_surfaceInfo.logicalHeight;
    m_renderer->resizeResources(m_surfaceInfo);
}

Renderer& Engine::rendererMut() {
    return *m_renderer;
}

RuntimeModel &Engine::runtimeModelMut() {
    return *m_runtimeModel;
}

RenderModel &Engine::renderModelMut() {
    return *m_renderModel;
}

CameraState& Engine::currentCameraStateMut() {
    return m_sceneManager.activeSceneMut().cameraStateMut();
}

}
