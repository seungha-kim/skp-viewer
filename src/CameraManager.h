#pragma once
#include <vector>
#include "CameraState.h"

class CameraManager {
public:
    // TODO: 데이터 노출
    CameraState& activeCamera();
    bool removeActiveCamera();
    void addCamera();
private:
    std::vector<CameraState> m_cameras = {CameraState()};
    int m_activeCameraIndex = 0;
};
