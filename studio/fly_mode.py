from dataclasses import dataclass
from enum import Flag, auto
import time
from typing import Union, Any

from PySide6.QtCore import Qt
from binding_test import CameraState

from .keymap import KeyMap


class FlyModeKeyCommand(Flag):
    NONE = 0

    FORWARD = auto()
    BACKWARD = auto()
    LEFT = auto()
    RIGHT = auto()
    DOWNWARD = auto()
    UPWARD = auto()


fly_mode_key_map = KeyMap((
    # (Qt.Key_Space, Qt.NoModifier, FlyModeKeyCommand.TOGGLE),
    (Qt.Key_W, Qt.NoModifier, FlyModeKeyCommand.FORWARD),
    (Qt.Key_S, Qt.NoModifier, FlyModeKeyCommand.BACKWARD),
    (Qt.Key_A, Qt.NoModifier, FlyModeKeyCommand.LEFT),
    (Qt.Key_D, Qt.NoModifier, FlyModeKeyCommand.RIGHT),
    (Qt.Key_Q, Qt.NoModifier, FlyModeKeyCommand.DOWNWARD),
    (Qt.Key_E, Qt.NoModifier, FlyModeKeyCommand.UPWARD),
))


class IdleState:
    pass


@dataclass()
class MovingState:
    last_time: float


AnyState = Union[IdleState, MovingState]


class FlyModeController:
    _running = FlyModeKeyCommand.NONE
    _state: AnyState = IdleState()
    _speed = 10

    def handle_key(self, key: Qt.Key, modifiers: Qt.KeyboardModifiers, is_pressed: bool) -> bool:
        if matched := fly_mode_key_map.match(modifiers, key):
            self._handle_command(matched, is_pressed)
            return True
        else:
            return False

    def should_continuously_render(self) -> bool:
        return isinstance(self._state, MovingState)

    def update(self, camera_state: CameraState) -> None:
        if self._running:
            current_time = time.time()
            if isinstance(self._state, IdleState):
                self._state = MovingState(current_time)
            self._move_camera(camera_state, current_time)
            self._state.last_time = current_time
        else:
            self._state = IdleState()

    def _handle_command(self, command: FlyModeKeyCommand, is_pressed: bool) -> None:
        if is_pressed:
            self._running |= command
        else:
            self._running &= ~command

    def _move_camera(self, camera_state: CameraState, current_time: float) -> None:
        assert isinstance(self._state, MovingState)
        delta_time = current_time - self._state.last_time
        delta = self._speed * delta_time
        if self._running & FlyModeKeyCommand.FORWARD:
            camera_state.pos += delta * camera_state.front()
        if self._running & FlyModeKeyCommand.BACKWARD:
            camera_state.pos -= delta * camera_state.front()
        if self._running & FlyModeKeyCommand.LEFT:
            camera_state.pos += delta * camera_state.left()
        if self._running & FlyModeKeyCommand.RIGHT:
            camera_state.pos -= delta * camera_state.left()
        if self._running & FlyModeKeyCommand.UPWARD:
            camera_state.pos += delta * camera_state.up
        if self._running & FlyModeKeyCommand.DOWNWARD:
            camera_state.pos -= delta * camera_state.up
