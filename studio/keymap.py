from collections import defaultdict
from typing import Optional, Iterable

from PySide6.QtCore import Qt
from binding_test import KeyCommand


class KeyMap:
    # NOTE: Qt.KeyboardModifiers 가 hashable 이 아니기 때문에 부득이 이런 구조로 함
    index: dict[Qt.Key, list[tuple[Qt.KeyboardModifier, KeyCommand]]]

    def __init__(self, keymap_list: Iterable[tuple[Qt.Key, Qt.KeyboardModifier, KeyCommand]]):
        self.index = defaultdict(list)
        for key, modifier, command in keymap_list:
            self.index[key].append((modifier, command))

    def match(self, modifiers: Qt.KeyboardModifiers, key: Qt.Key) -> Optional[KeyCommand]:
        if matched_tup := self.index.get(key):
            for mod, command in matched_tup:
                if mod == modifiers:
                    return command
        return None

    @classmethod
    def default(cls) -> 'KeyMap':
        return cls((
            (Qt.Key_Space, Qt.NoModifier, KeyCommand.FLY_MODE_TOGGLE),
            (Qt.Key_W, Qt.NoModifier, KeyCommand.FLY_MODE_FORWARD),
            (Qt.Key_S, Qt.NoModifier, KeyCommand.FLY_MODE_BACKWARD),
            (Qt.Key_A, Qt.NoModifier, KeyCommand.FLY_MODE_LEFT),
            (Qt.Key_D, Qt.NoModifier, KeyCommand.FLY_MODE_RIGHT),
            (Qt.Key_Q, Qt.NoModifier, KeyCommand.FLY_MODE_DOWNWARD),
            (Qt.Key_E, Qt.NoModifier, KeyCommand.FLY_MODE_UPWARD),
        ))
