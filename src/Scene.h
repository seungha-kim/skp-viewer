#pragma once
#include "graphics/DirectionalLight.h"
#include "CameraState.h"

class Scene final {
public:
    Scene() = default;
    [[nodiscard]] const DirectionalLight& sunLight() const;
    DirectionalLight& sunLightMut();
    [[nodiscard]] const CameraState& cameraState() const;
    CameraState& cameraStateMut();
private:
    DirectionalLight m_sunLight;
    CameraState m_camera;
};
