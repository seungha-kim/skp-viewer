#include "Scene.h"

namespace acon {

Scene::Scene() = default;

const DirectionalLight &Scene::sunLight() const {
    return m_sunLight;
}

DirectionalLight &Scene::sunLightMut() {
    return m_sunLight;
}

const CameraState &Scene::cameraState() const {
    return m_camera;
}

CameraState &Scene::cameraStateMut() {
    return m_camera;
}

void Scene::updateAspectRatio(const SurfaceInfo &surfaceInfo) {
    m_camera.aspectWidth = float(surfaceInfo.physicalWidth);
    m_camera.aspectHeight = float(surfaceInfo.physicalHeight);
}

}
