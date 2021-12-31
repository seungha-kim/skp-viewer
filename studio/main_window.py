from PySide6.QtCore import QSize
from PySide6.QtWidgets import QMainWindow, QPushButton, QVBoxLayout, QWidget

from .canvas_widget import CanvasWidget


class MainWindow(QMainWindow):
    def __init__(self) -> None:
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

        self.sizeHint = lambda: QSize(1366, 800)  # type: ignore

        fly_mode_toggle_button = QPushButton("Toggle")
        layout.addWidget(fly_mode_toggle_button)
        fly_mode_toggle_button.clicked.connect(self._handle_fly_mode_toggle_button_click)

        # Set the central widget of the Window.
        self.setCentralWidget(widget)

        self.canvas = canvas

    def handle_button_click(self):
        self.canvas.set_random_global_diffuse()

    def _handle_fly_mode_toggle_button_click(self) -> None:
        if self.canvas.is_in_fly_mode():
            self.canvas.turn_off_fly_mode()
        else:
            self.canvas.turn_on_fly_mode()
