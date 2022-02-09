from PySide6.QtCore import QSize
from PySide6.QtWidgets import QMainWindow, QPushButton, QVBoxLayout, QWidget, QHBoxLayout, QListView, QFileDialog

from binding_test import Engine
from .canvas_widget import CanvasWidget
from .hierarchy_panel import HierarchyPanelController
from .startup_options import StartupOptions
from .tag_panel_controller import TagPanelController
from .util import create_reader_by_file_format


class MainWindow(QMainWindow):
    def __init__(self, opts: StartupOptions) -> None:
        super().__init__()
        reader = create_reader_by_file_format(opts.model_path or self.__file_path_by_dialog())
        self._engine = Engine(reader)
        self._runtimeModel = self._engine.runtimeModelMut()
        self._reader = reader

        self.setWindowTitle("My App")
        root_layout = QVBoxLayout()

        toolbox_layout = QHBoxLayout()
        root_layout.addLayout(toolbox_layout)

        self.fly_mode_toggle_button = FlyModeToggleButton()
        toolbox_layout.addWidget(self.fly_mode_toggle_button)
        self.fly_mode_toggle_button.clicked.connect(self._handle_fly_mode_toggle_button_click)

        toolbox_layout.addStretch(1)

        content_layout = QHBoxLayout()
        root_layout.addLayout(content_layout)

        widget = QWidget()
        widget.setLayout(root_layout)

        left_area = QVBoxLayout()
        content_layout.addLayout(left_area)

        hierarchy_panel_ctrl = HierarchyPanelController()
        left_area.addWidget(hierarchy_panel_ctrl.widget)

        middle_layout = QVBoxLayout()
        content_layout.addLayout(middle_layout, 1)

        canvas = CanvasWidget(CanvasWidgetDelegateImpl(self), self._engine)
        middle_layout.addWidget(canvas)

        right_layout = QVBoxLayout()
        content_layout.addLayout(right_layout)

        tag_panel_ctrl = TagPanelController(self._runtimeModel, TagPanelControllerDelegateImpl(canvas))
        right_layout.addWidget(tag_panel_ctrl.widget)

        self.sizeHint = lambda: QSize(1366, 800)  # type: ignore

        # Set the central widget of the Window.
        self.setCentralWidget(widget)

        self.canvas = canvas

        self.statusBar().showMessage("Loaded", 3000)

    def _handle_fly_mode_toggle_button_click(self) -> None:
        if self.canvas.is_in_fly_mode():
            self.canvas.turn_off_fly_mode()
        else:
            self.canvas.turn_on_fly_mode()

    def __file_path_by_dialog(self) -> str:
        supported_formats = {
            "SketchUp": "*.skp",
            "Wavefront OBJ": "*.obj"
        }

        file_filter = ";;".join([
                                    "All supported formats (" + " ".join(supported_formats.values()) + ")"
                                ] + [
                                    f"{desc} ({ext})" for desc, ext in supported_formats.items()
                                ])
        file_path, selected_filter = QFileDialog.getOpenFileName(self, "Open File", "./", file_filter)
        return file_path


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


class TagPanelControllerDelegateImpl(TagPanelController.Delegate):
    def __init__(self, canvas_widget: CanvasWidget):
        self._canvas_widget = canvas_widget

    def render_immediately(self):
        self._canvas_widget.update()
