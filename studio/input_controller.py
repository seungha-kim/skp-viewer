from abc import *
from enum import Flag, auto
from PySide6.QtCore import Qt


class MouseButton(Flag):
    NONE = 0

    LEFT = auto()
    MIDDLE = auto()
    RIGHT = auto()

    @classmethod
    def from_qt_buttons(cls, buttons: Qt.MouseButtons) -> "MouseButton":
        result = cls.NONE
        if buttons & Qt.LeftButton:
            result |= cls.LEFT
        if buttons & Qt.MiddleButton:
            result |= cls.MIDDLE
        if buttons & Qt.RightButton:
            result |= cls.RIGHT
        return result


class AbstractInputController(ABC):
    def handle_key(
        self,
        key: Qt.Key,
        modifiers: Qt.KeyboardModifiers,
        pressed: bool,
    ) -> bool:
        pass

    def handle_mouse_move(
        self,
        x: int,
        y: int,
        width: int,
        height: int,
        buttons: MouseButton,
    ) -> None:
        pass

    def update(self) -> None:
        pass

    def should_render_continuously(self) -> bool:
        return False


class InputControllerOverriding(AbstractInputController):
    def __init__(
        self,
        overridden: AbstractInputController,
        overrider: AbstractInputController,
    ):
        self._overridden = overridden
        self._overrider = overrider

    def handle_key(
        self,
        key: Qt.Key,
        modifiers: Qt.KeyboardModifiers,
        pressed: bool,
    ) -> bool:
        return self._overrider.handle_key(
            key, modifiers, pressed
        ) or self._overridden.handle_key(key, modifiers, pressed)

    def handle_mouse_move(
        self,
        x: int,
        y: int,
        width: int,
        height: int,
        buttons: MouseButton,
    ) -> None:
        self._overrider.handle_mouse_move(x, y, width, height, buttons)

    def update(self) -> None:
        self._overridden.update()
        self._overrider.update()

    def should_render_continuously(self) -> bool:
        return (
            self._overrider.should_render_continuously()
            or self._overridden.should_render_continuously()
        )

    @property
    def overridden(self) -> AbstractInputController:
        return self._overridden
