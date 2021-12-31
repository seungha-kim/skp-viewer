from typing import Optional, Any, cast

import binding_test
from PySide6.QtCore import Qt, Signal, SignalInstance
import PySide6.QtGui
from PySide6.QtOpenGLWidgets import QOpenGLWidget

from studio.fly_mode import FlyModeController


class CanvasWidget(QOpenGLWidget):
    engine: binding_test.Engine
    fly_mode_controller: Optional[FlyModeController] = None
    fly_mode_toggled: Any = Signal(bool)

    def __init__(self) -> None:
        super().__init__()
        self.setFocusPolicy(Qt.FocusPolicy.StrongFocus)
        self.setUpdateBehavior(QOpenGLWidget.UpdateBehavior.NoPartialUpdate)

    def initializeGL(self) -> None:
        binding_test.init()
        self.engine = binding_test.Engine(self.surface_info())

    def paintGL(self) -> None:
        if fmc := self.fly_mode_controller:
            fmc.update(self.engine.currentCameraStateMut())
            if fmc.should_continuously_render():
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

    def keyPressEvent(self, event: PySide6.QtGui.QKeyEvent) -> None:
        self._dispatch_key_event(event.modifiers(), cast(Qt.Key, event.key()), True)

    def keyReleaseEvent(self, event: PySide6.QtGui.QKeyEvent) -> None:
        self._dispatch_key_event(event.modifiers(), cast(Qt.Key, event.key()), False)

    def _dispatch_key_event(self, modifiers: Qt.KeyboardModifiers, key: Qt.Key, pressed: bool) -> None:
        if fmc := self.fly_mode_controller:
            fmc.handle_key(key, modifiers, pressed)
        self.update()

    def set_random_global_diffuse(self) -> None:
        self.engine.setRandomGlobalDiffuse()
        self.update()

    def turn_on_fly_mode(self) -> None:
        self.fly_mode_controller = FlyModeController()
        self.fly_mode_toggled.emit(True)

    def turn_off_fly_mode(self) -> None:
        self.fly_mode_controller = None
        self.fly_mode_toggled.emit(False)

    def is_in_fly_mode(self) -> bool:
        return self.fly_mode_controller is not None
