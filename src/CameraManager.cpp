#include "CameraManager.h"

const CameraState &CameraManager::activeCamera() const {
    return m_cameras[m_activeCameraIndex];
}

CameraState &CameraManager::activeCameraMut() {
    return m_cameras[m_activeCameraIndex];
}

bool CameraManager::removeActiveCamera() {
    if (m_cameras.size() <= 1) {
        return false;
    }
    m_cameras.erase(m_cameras.begin() + m_activeCameraIndex);
    if (m_cameras.size() == m_activeCameraIndex) {
        m_activeCameraIndex--;
    }
    return true;
}

void CameraManager::addCamera() {
    m_cameras.emplace_back();
}

const std::vector<CameraState> &CameraManager::cameras() {
    return m_cameras;
}

bool CameraManager::setActiveCamera(int index) {
    if (0 <= index && index < m_cameras.size()) {
        m_activeCameraIndex = index;
        return true;
    }
    return false;
}

int CameraManager::activeCameraIndex() const {
    return m_activeCameraIndex;
}
