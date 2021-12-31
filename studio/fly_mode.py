from abc import *
from dataclasses import dataclass
from enum import Flag, auto
import time
from typing import Union

from PySide6.QtCore import Qt
from binding_test import CameraState

from .key_controller import AbstractKeyController
from .keymap import KeyMap


class FlyModeKeyCommand(Flag):
    NONE = 0

    FORWARD = auto()
    BACKWARD = auto()
    LEFT = auto()
    RIGHT = auto()
    DOWNWARD = auto()
    UPWARD = auto()

    EXIT = auto()


class State:
    class Idle:
        pass

    @dataclass()
    class Moving:
        last_time: float

    Any = Union[Idle, Moving]


class FlyModeController(AbstractKeyController):
    class Delegate(ABC):
        @abstractmethod
        def get_camera(self) -> CameraState:
            pass

        def on_exit(self) -> None:
            pass

        def on_moving_state_transition(self) -> None:
            pass

        def on_idle_state_transition(self) -> None:
            pass

    _running = FlyModeKeyCommand.NONE
    _state: State.Any = State.Idle()
    _speed = 10
    _key_map = KeyMap((
        (Qt.Key_W, Qt.NoModifier, FlyModeKeyCommand.FORWARD),
        (Qt.Key_S, Qt.NoModifier, FlyModeKeyCommand.BACKWARD),
        (Qt.Key_A, Qt.NoModifier, FlyModeKeyCommand.LEFT),
        (Qt.Key_D, Qt.NoModifier, FlyModeKeyCommand.RIGHT),
        (Qt.Key_Q, Qt.NoModifier, FlyModeKeyCommand.DOWNWARD),
        (Qt.Key_E, Qt.NoModifier, FlyModeKeyCommand.UPWARD),
        (Qt.Key_Escape, Qt.NoModifier, FlyModeKeyCommand.EXIT),
    ))

    def __init__(self, delegate: Delegate):
        self._delegate = delegate

    def handle_key(self, key: Qt.Key, modifiers: Qt.KeyboardModifiers, is_pressed: bool) -> bool:
        if matched := self._key_map.match(key, modifiers):
            self._handle_command(matched, is_pressed)
            return True
        else:
            return False

    def should_render_continuously(self) -> bool:
        return isinstance(self._state, State.Moving)

    def update(self) -> None:
        if self._running & FlyModeKeyCommand.EXIT:
            self._delegate.on_exit()
        elif self._running:
            current_time = time.time()
            if isinstance(self._state, State.Idle):
                self._state = State.Moving(current_time)
                self._delegate.on_moving_state_transition()
            self._move_camera(current_time)
            self._state.last_time = current_time
        else:
            self._state = State.Idle()
            self._delegate.on_idle_state_transition()

    def _handle_command(self, command: FlyModeKeyCommand, is_pressed: bool) -> None:
        if is_pressed:
            self._running |= command
        else:
            self._running &= ~command

    def _move_camera(self, current_time: float) -> None:
        assert isinstance(self._state, State.Moving)
        delta_time = current_time - self._state.last_time
        delta = self._speed * delta_time
        camera_state = self._delegate.get_camera()
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
