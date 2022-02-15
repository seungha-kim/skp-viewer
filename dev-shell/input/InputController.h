#pragma once

#include "../../engine/scene/SceneManager.h"
#include "FlyCameraContext.h"
#include "InputContext.h"
#include "ObjectEditContext.h"

class InputController {
public:
    void handleKeyboardInput(InputContext& ctx);
    void handleMouseInput(InputContext& ctx);
    void setGuiFocused(bool value);
    bool isCameraRotateMode();

private:
    bool m_guiFocused = false;
    bool m_cameraRotateMode = false;
    FlyCameraContext m_flyCameraContext;
    ObjectEditContext m_objectEditContext;
};
