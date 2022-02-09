from abc import *
from dataclasses import dataclass
from enum import Flag, auto
from typing import cast

from binding_test import CameraState, Engine, SurfaceInfo, init as init_engine
from PySide6.QtCore import Qt
from PySide6.QtGui import QKeyEvent
from PySide6.QtOpenGLWidgets import QOpenGLWidget

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
        def __init__(self, canvas_widget: 'CanvasWidget', engine: binding_test.Engine, parent: AbstractInputController):
            self.input_controller = InputControllerOverriding(
                overrider=FlyModeController(FlyModeControllerDelegateImpl(canvas_widget, engine)),
                overridden=parent,
            )


class CanvasWidget(QOpenGLWidget):
    class Delegate(ABC):
        def on_fly_mode_on(self):
            pass

        def on_fly_mode_off(self):
            pass

    def __init__(self, delegate: Delegate, engine: Engine) -> None:
        super().__init__()
        self._delegate = delegate
        self._engine = engine
        self._default_input_controller = CanvasInputController(self)
        self._state: State.Base = State.Default(self._default_input_controller)

        self.setFocusPolicy(Qt.FocusPolicy.StrongFocus)
        self.setUpdateBehavior(QOpenGLWidget.UpdateBehavior.NoPartialUpdate)

    def initializeGL(self) -> None:
        init_engine()
        self._engine.prepareToRender(self.surface_info())

    def paintGL(self) -> None:
        self._state.input_controller.update()
        if self._state.input_controller.should_render_continuously():
            self.update()
        self._engine.render(0)

    def resizeGL(self, w: int, h: int) -> None:
        self._engine.resize(self.surface_info())

    def surface_info(self) -> SurfaceInfo:
        w = self.size().width()
        h = self.size().height()
        screen = self.screen()
        device_pixel_ratio = screen.devicePixelRatio()
        pw = int(w * device_pixel_ratio)
        ph = int(h * device_pixel_ratio)
        return SurfaceInfo(w, h, pw, ph, device_pixel_ratio, device_pixel_ratio)

    def keyPressEvent(self, event: QKeyEvent) -> None:
        self._dispatch_key_event(event.modifiers(), cast(Qt.Key, event.key()), True)

    def keyReleaseEvent(self, event: QKeyEvent) -> None:
        self._dispatch_key_event(event.modifiers(), cast(Qt.Key, event.key()), False)

    def _dispatch_key_event(self, modifiers: Qt.KeyboardModifiers, key: Qt.Key, pressed: bool) -> None:
        if self._state.input_controller.handle_key(key, modifiers, pressed):
            self.update()

    def turn_on_fly_mode(self) -> None:
        if isinstance(self._state, State.Default):
            self._state = State.FlyMode(self, self._engine, self._default_input_controller)
        self._delegate.on_fly_mode_on()

    def turn_off_fly_mode(self) -> None:
        if isinstance(self._state, State.FlyMode):
            self._state = State.Default(self._default_input_controller)
        self._delegate.on_fly_mode_off()

    def is_in_fly_mode(self) -> bool:
        return isinstance(self._state, State.FlyMode)


class FlyModeControllerDelegateImpl(FlyModeController.Delegate):
    def __init__(self, canvas_widget: CanvasWidget, engine: Engine):
        self._canvas_widget = canvas_widget
        self._engine = engine

    def get_camera(self) -> CameraState:
        return self._engine.currentCameraStateMut()

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
