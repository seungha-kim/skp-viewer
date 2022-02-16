from abc import *
from dataclasses import dataclass
from enum import Flag, auto
import time
from typing import Union

from PySide6.QtCore import Qt
from binding_test import CameraState

from .input_controller import AbstractInputController, MouseButton
from .keymap import KeyMap
from .util import clamp


class FlyModeKeyCommand(Flag):
    NONE = 0

    FORWARD = auto()
    BACKWARD = auto()
    LEFT = auto()
    RIGHT = auto()
    DOWNWARD = auto()
    UPWARD = auto()

    PITCH_UP = auto()
    PITCH_DOWN = auto()
    YAW_LEFT = auto()
    YAW_RIGHT = auto()

    EXIT = auto()


class State:
    class Idle:
        pass

    @dataclass()
    class Moving:
        last_time: float

    Any = Union[Idle, Moving]


class FlyModeController(AbstractInputController):
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

    _prev_mouse_x = None
    _prev_mouse_y = None
    _running = FlyModeKeyCommand.NONE
    _state: State.Any = State.Idle()
    _move_speed = 7
    _rotation_speed = 50
    _mouse_yaw_speed = 0.3
    _mouse_pitch_speed = 0.3
    _pitch_limit = 80
    _key_map = KeyMap(
        (
            (Qt.Key_W, Qt.NoModifier, FlyModeKeyCommand.FORWARD),
            (Qt.Key_S, Qt.NoModifier, FlyModeKeyCommand.BACKWARD),
            (Qt.Key_A, Qt.NoModifier, FlyModeKeyCommand.LEFT),
            (Qt.Key_D, Qt.NoModifier, FlyModeKeyCommand.RIGHT),
            (Qt.Key_Q, Qt.NoModifier, FlyModeKeyCommand.DOWNWARD),
            (Qt.Key_E, Qt.NoModifier, FlyModeKeyCommand.UPWARD),
            (Qt.Key_Up, Qt.KeypadModifier, FlyModeKeyCommand.PITCH_UP),
            (Qt.Key_Down, Qt.KeypadModifier, FlyModeKeyCommand.PITCH_DOWN),
            (Qt.Key_Left, Qt.KeypadModifier, FlyModeKeyCommand.YAW_LEFT),
            (Qt.Key_Right, Qt.KeypadModifier, FlyModeKeyCommand.YAW_RIGHT),
            (Qt.Key_Escape, Qt.NoModifier, FlyModeKeyCommand.EXIT),
        )
    )

    def __init__(self, delegate: Delegate):
        self._delegate = delegate

    def handle_key(
        self,
        key: Qt.Key,
        modifiers: Qt.KeyboardModifiers,
        pressed: bool,
    ) -> bool:
        if matched := self._key_map.match(key, modifiers):
            self._handle_command(matched, pressed)
            return True
        else:
            return False

    def handle_mouse_move(
        self,
        x: int,
        y: int,
        width: int,
        height: int,
        buttons: MouseButton,
    ) -> None:
        # 마우스 커서를 숨기는 전형적 first person controller 시도해 봤는데,
        # 아래 글과 같이 뚝뚝 끊기는 문제가 있어서 일단 키보드 방향키로 카메라 회전
        # https://forum.qt.io/topic/123869/capturing-the-mouse-inside-a-qopenglwidget/10

        if not (buttons & MouseButton.RIGHT):
            return

        if self._prev_mouse_x is None:
            self._prev_mouse_x = x
            self._prev_mouse_y = y
            return
        delta_x = x - self._prev_mouse_x
        delta_y = y - self._prev_mouse_y
        self._prev_mouse_x = x
        self._prev_mouse_y = y
        camera = self._delegate.get_camera()
        camera.yaw -= delta_x * self._mouse_yaw_speed
        camera.pitch -= delta_y * self._mouse_pitch_speed
        camera.pitch = clamp(camera.pitch, -self._pitch_limit, self._pitch_limit)

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
        move_delta = self._move_speed * delta_time
        rotation_delta = self._rotation_speed * delta_time
        camera_state = self._delegate.get_camera()
        if self._running & FlyModeKeyCommand.FORWARD:
            camera_state.pos += move_delta * camera_state.front()
        if self._running & FlyModeKeyCommand.BACKWARD:
            camera_state.pos -= move_delta * camera_state.front()
        if self._running & FlyModeKeyCommand.LEFT:
            camera_state.pos += move_delta * camera_state.left()
        if self._running & FlyModeKeyCommand.RIGHT:
            camera_state.pos -= move_delta * camera_state.left()
        if self._running & FlyModeKeyCommand.UPWARD:
            camera_state.pos += move_delta * camera_state.up
        if self._running & FlyModeKeyCommand.DOWNWARD:
            camera_state.pos -= move_delta * camera_state.up
        if self._running & FlyModeKeyCommand.PITCH_UP:
            camera_state.pitch += rotation_delta
        if self._running & FlyModeKeyCommand.PITCH_DOWN:
            camera_state.pitch -= rotation_delta
        if self._running & FlyModeKeyCommand.YAW_LEFT:
            camera_state.yaw += rotation_delta
        if self._running & FlyModeKeyCommand.YAW_RIGHT:
            camera_state.yaw -= rotation_delta
