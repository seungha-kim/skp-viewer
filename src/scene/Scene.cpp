#include "Scene.h"

Scene::Scene(const SurfaceInfo &surfaceInfo) {
    m_camera.aspectWidth = surfaceInfo.logicalWidth;
    m_camera.aspectHeight = surfaceInfo.logicalHeight;
}

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
