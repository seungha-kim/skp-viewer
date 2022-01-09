import sys
import faulthandler

from PySide6.QtGui import QSurfaceFormat
from PySide6.QtWidgets import QApplication

from studio.startup_options import StartupOptions
from .main_window import MainWindow


faulthandler.enable()


class App:
    def __init__(self, opts: StartupOptions):
        self.opts = opts
        self.setup_opengl()
        self.app = QApplication(sys.argv)
        self.window = MainWindow(opts)
        self.window.show()

    def run(self):
        self.app.exec()

    @staticmethod
    def setup_opengl():
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


def main():
    app = App(StartupOptions())
    app.run()


if __name__ == "__main__":
    main()