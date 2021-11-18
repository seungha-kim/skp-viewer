#include "Scene.h"


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
