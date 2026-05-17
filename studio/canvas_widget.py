import math
from abc import *
from dataclasses import dataclass
from enum import Flag, auto
from typing import cast

from binding_test import CameraState, Engine, SurfaceInfo, init as init_engine
from PySide6.QtCore import Qt
from PySide6.QtGui import QKeyEvent, QMouseEvent, QWheelEvent
from PySide6.QtOpenGLWidgets import QOpenGLWidget

from .fly_mode import FlyModeController
from .input_controller import (
    AbstractInputController,
    InputControllerOverriding,
    MouseButton,
)
from .keymap import KeyMap
from .util import clamp


class State:
    class Base(ABC):
        input_controller: AbstractInputController

    @dataclass()
    class Default(Base):
        input_controller: AbstractInputController

    class FlyMode(Base):
        def __init__(
            self,
            canvas_widget: "CanvasWidget",
            engine: Engine,
            parent: AbstractInputController,
        ):
            self.prev_mouse_x = 0
            self.prev_mouse_y = 0
            self.input_controller = InputControllerOverriding(
                overrider=FlyModeController(
                    FlyModeControllerDelegateImpl(canvas_widget, engine)
                ),
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
        self._default_input_controller = CanvasInputController(self, engine)
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

    def mousePressEvent(self, event: QMouseEvent) -> None:
        if event.button() in (Qt.MouseButton.LeftButton, Qt.MouseButton.MiddleButton):
            self._default_input_controller.reset_mouse_tracking()
        if event.button() == Qt.MouseButton.RightButton:
            self.turn_on_fly_mode()
        # Grab focus so keyboard shortcuts work after clicking the canvas
        self.setFocus()

    def mouseReleaseEvent(self, event: QMouseEvent) -> None:
        if event.button() == Qt.MouseButton.RightButton:
            self.turn_off_fly_mode()

    def wheelEvent(self, event: QWheelEvent) -> None:
        delta = event.angleDelta().y()
        if delta != 0:
            self._default_input_controller.handle_wheel(delta)
            self.update()

    def mouseMoveEvent(self, event: QMouseEvent) -> None:
        pos = event.pos()
        buttons = MouseButton.from_qt_buttons(event.buttons())
        # Shift+Left acts as Middle-drag (pan) for laptops without a middle button
        if (buttons & MouseButton.LEFT) and (event.modifiers() & Qt.ShiftModifier):
            buttons = (buttons & ~MouseButton.LEFT) | MouseButton.MIDDLE
        width = self.width()
        height = self.height()
        self._state.input_controller.handle_mouse_move(
            pos.x(), pos.y(), width, height, buttons
        )
        self.update()

    def _dispatch_key_event(
        self,
        modifiers: Qt.KeyboardModifiers,
        key: Qt.Key,
        pressed: bool,
    ) -> None:
        if self._state.input_controller.handle_key(key, modifiers, pressed):
            self.update()

    def turn_on_fly_mode(self) -> None:
        if isinstance(self._state, State.Default):
            self._state = State.FlyMode(
                self, self._engine, self._default_input_controller
            )
        self.setCursor(Qt.CrossCursor)
        self._delegate.on_fly_mode_on()

    def turn_off_fly_mode(self) -> None:
        if isinstance(self._state, State.FlyMode):
            self._state = State.Default(self._default_input_controller)
        self.setCursor(Qt.ArrowCursor)
        self._delegate.on_fly_mode_off()

    def is_in_fly_mode(self) -> bool:
        return isinstance(self._state, State.FlyMode)

    def focus_on_object(self, object_id: int) -> None:
        center = self._engine.getObjectWorldCenter(object_id)
        if center is not None:
            self._default_input_controller.focus_on(center)
            self.update()


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
    _keymap = KeyMap(
        ((Qt.Key_AsciiTilde, Qt.ShiftModifier, CanvasKeyCommand.FLY_MODE),)
    )

    _ORBIT_SENSITIVITY = 0.3
    _PAN_SENSITIVITY = 0.01
    _ZOOM_SENSITIVITY = 0.002
    _MIN_ORBIT_DISTANCE = 0.5

    def __init__(self, canvas: CanvasWidget, engine: Engine):
        self._canvas = canvas
        self._engine = engine
        self._prev_mouse_x: int | None = None
        self._prev_mouse_y: int | None = None
        self._orbit_distance: float = 10.0
        self._orbit_initialized = False

    def _ensure_orbit_initialized(self):
        if not self._orbit_initialized:
            camera = self._engine.currentCameraStateMut()
            pos = camera.pos
            dist = math.sqrt(pos.x ** 2 + pos.y ** 2 + pos.z ** 2)
            self._orbit_distance = max(dist, 1.0)
            self._orbit_initialized = True

    def reset_mouse_tracking(self):
        self._prev_mouse_x = None
        self._prev_mouse_y = None

    def focus_on(self, center) -> None:
        self._ensure_orbit_initialized()
        camera = self._engine.currentCameraStateMut()
        front = camera.front()
        # Move camera back from the center point along the front vector
        camera.pos = center - front * self._orbit_distance

    def handle_key(
        self,
        key: Qt.Key,
        modifiers: Qt.KeyboardModifiers,
        pressed: bool,
    ) -> bool:
        if matched := self._keymap.match(key, modifiers):
            if matched == CanvasKeyCommand.FLY_MODE and pressed:
                self._canvas.turn_on_fly_mode()
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
        if not (buttons & MouseButton.LEFT or buttons & MouseButton.MIDDLE):
            self._prev_mouse_x = None
            self._prev_mouse_y = None
            return

        if self._prev_mouse_x is None:
            self._prev_mouse_x = x
            self._prev_mouse_y = y
            return

        delta_x = x - self._prev_mouse_x
        delta_y = y - self._prev_mouse_y
        self._prev_mouse_x = x
        self._prev_mouse_y = y

        if delta_x == 0 and delta_y == 0:
            return

        self._ensure_orbit_initialized()
        camera = self._engine.currentCameraStateMut()

        if buttons & MouseButton.LEFT:
            # Orbit: rotate camera around the target point
            front = camera.front()
            target = camera.pos + self._orbit_distance * front

            camera.yaw -= delta_x * self._ORBIT_SENSITIVITY
            camera.pitch = clamp(
                camera.pitch - delta_y * self._ORBIT_SENSITIVITY,
                -89.0, 89.0,
            )

            new_front = camera.front()
            camera.pos = target - self._orbit_distance * new_front

        elif buttons & MouseButton.MIDDLE:
            # Pan: translate camera perpendicular to view direction
            pan_scale = self._PAN_SENSITIVITY * self._orbit_distance
            left = camera.left()
            up = camera.up
            camera.pos = (
                camera.pos
                + (delta_x * pan_scale) * left
                + (delta_y * pan_scale) * up
            )

    def handle_wheel(self, delta: int) -> None:
        self._ensure_orbit_initialized()
        camera = self._engine.currentCameraStateMut()
        zoom_amount = delta * self._ZOOM_SENSITIVITY * self._orbit_distance
        front = camera.front()
        camera.pos = camera.pos + zoom_amount * front
        self._orbit_distance = max(
            self._orbit_distance - zoom_amount,
            self._MIN_ORBIT_DISTANCE,
        )
