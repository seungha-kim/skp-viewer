from collections import defaultdict
from typing import Optional, Iterable, Generic, TypeVar, Any

from PySide6.QtCore import Qt


KeyCommand = TypeVar('KeyCommand')


class KeyMap(Generic[KeyCommand]):
    # NOTE: Qt.KeyboardModifiers 가 hashable 이 아니기 때문에 부득이 이런 구조로 함
    index: dict[Qt.Key, list[tuple[Qt.KeyboardModifier, KeyCommand]]]

    def __init__(self, keymap_list: Iterable[tuple[Qt.Key, Qt.KeyboardModifier, KeyCommand]]):
        self.index = defaultdict(list)
        for key, modifier, command in keymap_list:
            self.index[key].append((modifier, command))

    def match(self, key: Any, modifiers: Any) -> Optional[KeyCommand]:
        if matched_tup := self.index.get(key):
            for mod, command in matched_tup:
                if mod == modifiers:
                    return command
        return None
