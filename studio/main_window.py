from PySide6.QtCore import QSize
from PySide6.QtWidgets import QMainWindow, QPushButton, QVBoxLayout, QWidget

from .canvas_widget import CanvasWidget


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
