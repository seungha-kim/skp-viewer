from abc import *
from PySide6.QtCore import Qt


class AbstractInputController(ABC):
    def handle_key(self, key: Qt.Key, modifiers: Qt.KeyboardModifiers, pressed: bool) -> bool:
        pass

    def update(self) -> None:
        pass

    def should_render_continuously(self) -> bool:
        return False


class InputControllerOverriding(AbstractInputController):
    def __init__(self, overridden: AbstractInputController, overrider: AbstractInputController):
        self._overridden = overridden
        self._overrider = overrider

    def handle_key(self, key: Qt.Key, modifiers: Qt.KeyboardModifiers, pressed: bool) -> bool:
        return (
                self._overrider.handle_key(key, modifiers, pressed)
                or self._overridden.handle_key(key, modifiers, pressed)
        )

    def update(self) -> None:
        self._overridden.update()
        self._overrider.update()

    def should_render_continuously(self) -> bool:
        return self._overrider.should_render_continuously() or self._overridden.should_render_continuously()

    @property
    def overridden(self) -> AbstractInputController:
        return self._overridden
