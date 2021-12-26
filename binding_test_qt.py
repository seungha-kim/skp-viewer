import sys
import faulthandler
from typing import Optional

from PySide6.QtCore import QSize, Qt, QElapsedTimer
from PySide6.QtGui import QSurfaceFormat
from PySide6.QtWidgets import QApplication, QMainWindow, QPushButton, QVBoxLayout, QWidget
from PySide6.QtOpenGLWidgets import QOpenGLWidget
import PySide6.QtGui
import binding_test
from binding_test import KeyCommand

faulthandler.enable()


class KeyMap:
    def match(self, modifiers: Qt.KeyboardModifiers, key: Qt.Key) -> Optional[KeyCommand]:
        if matched_tup := self.keymap.get(key):
            for mod, command in matched_tup:
                if mod == modifiers:
                    return command
        return None

    keymap = {
        Qt.Key_Space: (
            (Qt.NoModifier, KeyCommand.FLY_MODE_TOGGLE),
        ),
        Qt.Key_W: (
            (Qt.NoModifier, KeyCommand.FLY_MODE_FORWARD),
        ),
        Qt.Key_S: (
            (Qt.NoModifier, KeyCommand.FLY_MODE_BACKWARD),
        ),
        Qt.Key_A: (
            (Qt.NoModifier, KeyCommand.FLY_MODE_LEFT),
        ),
        Qt.Key_D: (
            (Qt.NoModifier, KeyCommand.FLY_MODE_RIGHT),
        ),
        Qt.Key_Q: (
            (Qt.NoModifier, KeyCommand.FLY_MODE_DOWNWARD),
        ),
        Qt.Key_E: (
            (Qt.NoModifier, KeyCommand.FLY_MODE_UPWARD),
        ),
    }


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


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("My App")

        widget = QWidget()
        layout = QVBoxLayout()
        widget.setLayout(layout)

        canvas = CanvasWidget()
        layout.addWidget(canvas)

        button = QPushButton("Press Me!")
        layout.addWidget(button)
        button.clicked.connect(self.handle_button_click)

        self.sizeHint = lambda: QSize(1366, 800)

        # Set the central widget of the Window.
        self.setCentralWidget(widget)

        self.canvas = canvas

    def handle_button_click(self):
        self.canvas.set_random_global_diffuse()


def main():
    gl_format = QSurfaceFormat()
    gl_format.setProfile(QSurfaceFormat.CoreProfile)
    gl_format.setVersion(3, 3)
    gl_format.setDepthBufferSize(24)
    gl_format.setStencilBufferSize(8)
    gl_format.setRedBufferSize(8)
    gl_format.setGreenBufferSize(8)
    gl_format.setBlueBufferSize(8)
    gl_format.setAlphaBufferSize(8)
    gl_format.setRenderableType(QSurfaceFormat.OpenGL)
    QSurfaceFormat.setDefaultFormat(gl_format)

    app = QApplication(sys.argv)

    window = MainWindow()
    window.show()

    app.exec()


if __name__ == "__main__":
    main()