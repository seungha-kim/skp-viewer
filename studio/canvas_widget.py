from abc import *
from dataclasses import dataclass
from enum import Flag, auto
from typing import cast

import binding_test
from PySide6.QtCore import Qt
from PySide6.QtGui import QKeyEvent
from PySide6.QtOpenGLWidgets import QOpenGLWidget

from binding_test import CameraState
from .fly_mode import FlyModeController
from .input_controller import AbstractInputController, InputControllerOverriding
from .keymap import KeyMap


class State:
    class Base(ABC):
        input_controller: AbstractInputController

    @dataclass()
    class Default(Base):
        input_controller: AbstractInputController

    class FlyMode(Base):
        def __init__(self, canvas_widget: 'CanvasWidget', parent: AbstractInputController):
            self.input_controller = InputControllerOverriding(
                overrider=FlyModeController(FlyModeControllerDelegateImpl(canvas_widget)),
                overridden=parent,
            )


class CanvasWidget(QOpenGLWidget):
    class Delegate(ABC):
        def on_fly_mode_on(self):
            pass

        def on_fly_mode_off(self):
            pass

    engine: binding_test.Engine

    def __init__(self, delegate: Delegate) -> None:
        super().__init__()
        self._delegate = delegate
        self._default_input_controller = CanvasInputController(self)
        self._state: State.Base = State.Default(self._default_input_controller)

        self.setFocusPolicy(Qt.FocusPolicy.StrongFocus)
        self.setUpdateBehavior(QOpenGLWidget.UpdateBehavior.NoPartialUpdate)

    def initializeGL(self) -> None:
        binding_test.init()
        self.engine = binding_test.Engine(self.surface_info())

    def paintGL(self) -> None:
        self._state.input_controller.update()
        if self._state.input_controller.should_render_continuously():
            self.update()
        self.engine.render(0)

    def resizeGL(self, w: int, h: int) -> None:
        self.engine.resize(self.surface_info())

    def surface_info(self) -> binding_test.SurfaceInfo:
        w = self.size().width()
        h = self.size().height()
        screen = self.screen()
        device_pixel_ratio = screen.devicePixelRatio()
        pw = int(w * device_pixel_ratio)
        ph = int(h * device_pixel_ratio)
        return binding_test.SurfaceInfo(w, h, pw, ph, device_pixel_ratio, device_pixel_ratio)

    def keyPressEvent(self, event: QKeyEvent) -> None:
        self._dispatch_key_event(event.modifiers(), cast(Qt.Key, event.key()), True)

    def keyReleaseEvent(self, event: QKeyEvent) -> None:
        self._dispatch_key_event(event.modifiers(), cast(Qt.Key, event.key()), False)

    def _dispatch_key_event(self, modifiers: Qt.KeyboardModifiers, key: Qt.Key, pressed: bool) -> None:
        if self._state.input_controller.handle_key(key, modifiers, pressed):
            self.update()

    def set_random_global_diffuse(self) -> None:
        self.engine.setRandomGlobalDiffuse()
        self.update()

    def turn_on_fly_mode(self) -> None:
        if isinstance(self._state, State.Default):
            self._state = State.FlyMode(self, self._default_input_controller)
        self._delegate.on_fly_mode_on()

    def turn_off_fly_mode(self) -> None:
        if isinstance(self._state, State.FlyMode):
            self._state = State.Default(self._default_input_controller)
        self._delegate.on_fly_mode_off()

    def is_in_fly_mode(self) -> bool:
        return isinstance(self._state, State.FlyMode)


class FlyModeControllerDelegateImpl(FlyModeController.Delegate):
    def __init__(self, canvas_widget: CanvasWidget):
        self._canvas_widget = canvas_widget

    def get_camera(self) -> CameraState:
        return self._canvas_widget.engine.currentCameraStateMut()

    def on_exit(self) -> None:
        self._canvas_widget.turn_off_fly_mode()


class CanvasKeyCommand(Flag):
    NONE = 0

    FLY_MODE = auto()


class CanvasInputController(AbstractInputController):
    _keymap = KeyMap((
        (Qt.Key_AsciiTilde, Qt.ShiftModifier, CanvasKeyCommand.FLY_MODE),
    ))

    def __init__(self, canvas: CanvasWidget):
        self._canvas = canvas

    def handle_key(self, key: Qt.Key, modifiers: Qt.KeyboardModifiers, pressed: bool) -> bool:
        if matched := self._keymap.match(key, modifiers):
            if matched == CanvasKeyCommand.FLY_MODE and pressed:
                self._canvas.turn_on_fly_mode()
            return True
        else:
            return False
