#pragma once


#include <GLFW/glfw3.h>
#include "SceneManager.h"
#include "InputContext.h"
#include "FlyCameraContext.h"
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
    int m_prevSpaceState = GLFW_RELEASE;
    FlyCameraContext m_flyCameraContext;
    ObjectEditContext m_objectEditContext;
};
