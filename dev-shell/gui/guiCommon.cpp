#include "guiCommon.h"

ImGuiWindowFlags windowFlag(GuiContext& ctx) {
    ImGuiWindowFlags flag = 0;
    if (ctx.inputController.isCameraRotateMode()) {
        flag |= ImGuiWindowFlags_NoInputs;
    }
    return flag;
}