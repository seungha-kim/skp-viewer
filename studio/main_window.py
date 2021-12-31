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

        canvas = CanvasWidget(CanvasWidgetDelegateImpl(self))
        layout.addWidget(canvas)

        button = QPushButton("Press Me!")
        layout.addWidget(button)
        button.clicked.connect(self._handle_button_click)

        self.sizeHint = lambda: QSize(1366, 800)  # type: ignore

        self.fly_mode_toggle_button = FlyModeToggleButton()
        layout.addWidget(self.fly_mode_toggle_button)
        self.fly_mode_toggle_button.clicked.connect(self._handle_fly_mode_toggle_button_click)

        # Set the central widget of the Window.
        self.setCentralWidget(widget)

        self.canvas = canvas

    def _handle_button_click(self):
        self.canvas.set_random_global_diffuse()

    def _handle_fly_mode_toggle_button_click(self) -> None:
        if self.canvas.is_in_fly_mode():
            self.canvas.turn_off_fly_mode()
        else:
            self.canvas.turn_on_fly_mode()


class FlyModeToggleButton(QPushButton):
    label_turn_on = "Fly Mode On"
    label_turn_off = "Fly Mode Off"

    def __init__(self):
        super().__init__()
        self.setText(self.label_turn_on)

    def change_text(self, on: bool):
        if on:
            self.setText(self.label_turn_on)
        else:
            self.setText(self.label_turn_off)


class CanvasWidgetDelegateImpl(CanvasWidget.Delegate):
    def __init__(self, main_window: MainWindow):
        self._main_window = main_window

    def on_fly_mode_on(self):
        self._main_window.fly_mode_toggle_button.change_text(on=False)

    def on_fly_mode_off(self):
        self._main_window.fly_mode_toggle_button.change_text(on=True)
