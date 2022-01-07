#pragma once
#include "DirectionalLight.h"
#include "CameraState.h"
#include "../render/SurfaceInfo.h"

namespace acon {

class Scene final {
public:
    Scene(const SurfaceInfo& surfaceInfo);
    [[nodiscard]] const DirectionalLight& sunLight() const;
    DirectionalLight& sunLightMut();
    [[nodiscard]] const CameraState& cameraState() const;
    CameraState& cameraStateMut();
private:
    DirectionalLight m_sunLight;
    CameraState m_camera;
};

}
