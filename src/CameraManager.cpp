#include "CameraManager.h"

CameraState &CameraManager::activeCamera() {
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
