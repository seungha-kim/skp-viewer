#pragma once
#include <vector>
#include "CameraState.h"

class CameraManager {
public:
    [[nodiscard]] const CameraState& activeCamera() const;
    CameraState& activeCameraMut();
    int activeCameraIndex() const;
    bool removeActiveCamera();
    void addCamera();
    const std::vector<CameraState>& cameras();
    bool setActiveCamera(int index);
private:
    std::vector<CameraState> m_cameras = {CameraState()};
    int m_activeCameraIndex = 0;
};
