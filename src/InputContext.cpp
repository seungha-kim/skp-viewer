#include "InputContext.h"

bool InputContext::isBeingPressed(KeyCommand keyCommand) {
    auto i = static_cast<unsigned long>(keyCommand);
    return keyCommandSet[i];
}

bool InputContext::isJustPressed(KeyCommand keyCommand) {
    auto i = static_cast<unsigned long>(keyCommand);
    return !prevKeyCommandSet[i] && keyCommandSet[i];
}

void KeyCommandSetManipulator::set(KeyCommand keyCommand, bool value) {
    auto i = static_cast<unsigned long>(keyCommand);
    keyCommandSet[i] = value;
}
