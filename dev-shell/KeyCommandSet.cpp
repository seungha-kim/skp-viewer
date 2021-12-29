#include "KeyCommandSet.h"

void KeyCommandSet::setPressed(KeyCommand keyCommand, bool value) {
    auto i = static_cast<unsigned long>(keyCommand);
    m_data[i] = value;
}

bool KeyCommandSet::getPressed(KeyCommand keyCommand) const {
    auto i = static_cast<unsigned long>(keyCommand);
    return m_data[i];
}
