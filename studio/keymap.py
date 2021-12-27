from typing import Optional

from PySide6.QtCore import Qt
from binding_test import KeyCommand


class KeyMap:
    def match(self, modifiers: Qt.KeyboardModifiers, key: Qt.Key) -> Optional[KeyCommand]:
        if matched_tup := self.keymap.get(key):
            for mod, command in matched_tup:
                if mod == modifiers:
                    return command
        return None

    keymap = {
        Qt.Key_Space: (
            (Qt.NoModifier, KeyCommand.FLY_MODE_TOGGLE),
        ),
        Qt.Key_W: (
            (Qt.NoModifier, KeyCommand.FLY_MODE_FORWARD),
        ),
        Qt.Key_S: (
            (Qt.NoModifier, KeyCommand.FLY_MODE_BACKWARD),
        ),
        Qt.Key_A: (
            (Qt.NoModifier, KeyCommand.FLY_MODE_LEFT),
        ),
        Qt.Key_D: (
            (Qt.NoModifier, KeyCommand.FLY_MODE_RIGHT),
        ),
        Qt.Key_Q: (
            (Qt.NoModifier, KeyCommand.FLY_MODE_DOWNWARD),
        ),
        Qt.Key_E: (
            (Qt.NoModifier, KeyCommand.FLY_MODE_UPWARD),
        ),
    }

