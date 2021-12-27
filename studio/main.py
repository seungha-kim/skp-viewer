import sys
import faulthandler

from PySide6.QtGui import QSurfaceFormat
from PySide6.QtWidgets import QApplication

from .main_window import MainWindow


faulthandler.enable()


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
