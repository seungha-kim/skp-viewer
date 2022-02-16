from PySide6.QtCore import QSize
from PySide6.QtWidgets import (
    QMainWindow,
    QVBoxLayout,
    QWidget,
    QHBoxLayout,
    QFileDialog,
)

from binding_test import Engine
from .canvas_widget import CanvasWidget
from .hierarchy_panel import HierarchyPanelController
from .startup_options import StartupOptions
from .tag_panel_controller import TagPanelController
from .util import create_reader_by_file_format


class MainWindow(QMainWindow):
    def __init__(self, opts: StartupOptions) -> None:
        super().__init__()
        reader = create_reader_by_file_format(
            opts.model_path or self.__file_path_by_dialog()
        )
        self._engine = Engine(reader)
        self._runtimeModel = self._engine.runtimeModelMut()
        self._reader = reader

        self.setWindowTitle("SKP Viewer")
        root_layout = QVBoxLayout()

        content_layout = QHBoxLayout()
        root_layout.addLayout(content_layout)

        widget = QWidget()
        widget.setLayout(root_layout)

        left_area = QVBoxLayout()
        content_layout.addLayout(left_area)

        middle_layout = QVBoxLayout()
        content_layout.addLayout(middle_layout, 1)

        right_layout = QVBoxLayout()
        content_layout.addLayout(right_layout)

        canvas = CanvasWidget(CanvasWidgetDelegateImpl(self), self._engine)
        middle_layout.addWidget(canvas)

        hierarchy_panel_ctrl_d = HierarchyPanelControllerDelegate(canvas)
        hierarchy_panel_ctrl = HierarchyPanelController(
            hierarchy_panel_ctrl_d, self._runtimeModel
        )
        left_area.addWidget(hierarchy_panel_ctrl.widget)

        tag_panel_ctrl = TagPanelController(
            self._runtimeModel, TagPanelControllerDelegateImpl(canvas)
        )
        right_layout.addWidget(tag_panel_ctrl.widget)

        self.sizeHint = lambda: QSize(1366, 800)  # type: ignore

        # Set the central widget of the Window.
        self.setCentralWidget(widget)

        self.canvas = canvas

        self.statusBar().showMessage("Loaded", 3000)

    def __file_path_by_dialog(self) -> str:
        supported_formats = {"SketchUp": "*.skp", "Wavefront OBJ": "*.obj"}

        file_filter = ";;".join(
            ["All supported formats (" + " ".join(supported_formats.values()) + ")"]
            + [f"{desc} ({ext})" for desc, ext in supported_formats.items()]
        )
        file_path, selected_filter = QFileDialog.getOpenFileName(
            self, "Open File", "./", file_filter
        )
        return file_path


class CanvasWidgetDelegateImpl(CanvasWidget.Delegate):
    def __init__(self, main_window: MainWindow):
        self._main_window = main_window

    def on_fly_mode_on(self):
        self._main_window.statusBar().showMessage("Fly mode turned on", 3000)

    def on_fly_mode_off(self):
        self._main_window.statusBar().showMessage("Fly mode turned off", 3000)


class TagPanelControllerDelegateImpl(TagPanelController.Delegate):
    def __init__(self, canvas_widget: CanvasWidget):
        self._canvas_widget = canvas_widget

    def render_immediately(self):
        self._canvas_widget.update()


class HierarchyPanelControllerDelegate(HierarchyPanelController.Delegate):
    def __init__(self, canvas_widget: CanvasWidget):
        self._canvas_widget = canvas_widget

    def render_immediately(self):
        self._canvas_widget.update()
