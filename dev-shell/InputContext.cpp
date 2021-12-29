#include "InputContext.h"

bool InputContext::isBeingPressed(KeyCommand keyCommand) {
    return keyCommandSet.getPressed(keyCommand);
}

bool InputContext::isJustPressed(KeyCommand keyCommand) {
    return !prevKeyCommandSet.getPressed(keyCommand) && keyCommandSet.getPressed(keyCommand);
}
