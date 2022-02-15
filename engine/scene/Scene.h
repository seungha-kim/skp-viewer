#pragma once
#include "../render/SurfaceInfo.h"
#include "CameraState.h"
#include "DirectionalLight.h"

namespace acon {

class Scene final {
public:
    Scene();
    [[nodiscard]] const DirectionalLight& sunLight() const;
    DirectionalLight& sunLightMut();
    [[nodiscard]] const CameraState& cameraState() const;
    CameraState& cameraStateMut();
    void updateAspectRatio(const SurfaceInfo& surfaceInfo);

private:
    DirectionalLight m_sunLight;
    CameraState m_camera;
};

}
