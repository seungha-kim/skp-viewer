import sys
import faulthandler

from PySide6.QtCore import QSize
from PySide6.QtGui import QSurfaceFormat
from PySide6.QtWidgets import QApplication, QMainWindow, QPushButton, QVBoxLayout, QWidget
from PySide6.QtOpenGLWidgets import QOpenGLWidget
import binding_test


faulthandler.enable()


class CanvasWidget(QOpenGLWidget):
    engine = None

    def initializeGL(self) -> None:
        binding_test.init()
        self.engine = binding_test.Engine(self.surface_info())

    def paintGL(self) -> None:
        self.engine.render(0)

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