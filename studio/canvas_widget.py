import binding_test
from PySide6.QtCore import QElapsedTimer, Qt
import PySide6.QtGui
from PySide6.QtOpenGLWidgets import QOpenGLWidget

from .keymap import KeyMap


class CanvasWidget(QOpenGLWidget):
    engine = None
    key_command_set = binding_test.KeyCommandSet()
    timer = QElapsedTimer()
    keymap = KeyMap()

    def __init__(self):
        super().__init__()
        self.setFocusPolicy(Qt.FocusPolicy.StrongFocus)
        self.timer.start()
        self.setUpdateBehavior(QOpenGLWidget.UpdateBehavior.NoPartialUpdate)

    def initializeGL(self) -> None:
        binding_test.init()
        self.engine = binding_test.Engine(self.surface_info())

    def paintGL(self) -> None:
        self.engine.handleInput()
        self.engine.updateTime(float(self.timer.elapsed() / 1000))
        self.engine.render(0)
        if self.engine.shouldContinuouslyRender():
            self.update()

    def resizeGL(self, w: int, h: int) -> None:
        self.engine.resize(self.surface_info())

    def surface_info(self):
        w = self.size().width()
        h = self.size().height()
        screen = self.screen()
        device_pixel_ratio = screen.devicePixelRatio()
        pw = int(w * device_pixel_ratio)
        ph = int(h * device_pixel_ratio)
        return binding_test.SurfaceInfo(w, h, pw, ph, device_pixel_ratio, device_pixel_ratio)

    def keyPressEvent(self, event: PySide6.QtGui.QKeyEvent) -> None:
        self._dispatch_key_event(event.modifiers(), event.key(), True)

    def keyReleaseEvent(self, event: PySide6.QtGui.QKeyEvent) -> None:
        self._dispatch_key_event(event.modifiers(), event.key(), False)

    def _dispatch_key_event(self, modifiers: Qt.KeyboardModifiers, key: Qt.Key, pressed: bool):
        if command := self.keymap.match(modifiers, key):
            self.key_command_set.setPressed(command, pressed)
            self.engine.onKeyboardStateChange(self.key_command_set)
            self.update()

    def set_random_global_diffuse(self):
        self.engine.setRandomGlobalDiffuse()
        self.update()
