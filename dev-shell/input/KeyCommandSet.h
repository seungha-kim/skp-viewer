#pragma once
#include "KeyCommand.h"
#include <bitset>

/// 효율적으로 복사됨.
class KeyCommandSet {
public:
    void setPressed(KeyCommand keyCommand, bool value);
    [[nodiscard]] bool getPressed(KeyCommand keyCommand) const;

private:
    std::bitset<static_cast<unsigned long>(KeyCommand::END_OF_COMMAND)> m_data;
};
