from typing import Any

from PySide6.QtCore import QAbstractListModel, Qt, QModelIndex
from PySide6.QtWidgets import QListView
from binding_test import RuntimeModel


class TagListView(QListView):
    class Delegate:
        def toggle_visibility(self, index: int):
            pass

    def __init__(self, delegate: Delegate):
        super().__init__()
        self._delegate = delegate
        self.clicked.connect(self.handle_click)

    def handle_click(self, index):
        self._delegate.toggle_visibility(index.row())


class TagListModel(QAbstractListModel):
    def __init__(self, model: RuntimeModel):
        super().__init__()
        self.model = model

    def data(self, index: QModelIndex, role: int) -> Any:
        if role == Qt.DisplayRole:
            tag_id = self.model.getTag(index.row())
            visible = self.model.getTagVisibility(index.row())
            visible_str = "[O]" if visible else "[X]"
            return visible_str + self.model.getTagName(tag_id)

    def rowCount(self, index: QModelIndex) -> int:
        return self.model.getTagCount()


class TagPanelController(TagListView.Delegate):
    class Delegate:
        def render_immediately(self):
            pass

    def __init__(self, model: RuntimeModel, delegate: Delegate):
        self._model = model
        self._delegate = delegate
        self._widget = TagListView(self)
        self._listModel = TagListModel(model)
        self._widget.setModel(self._listModel)

    @property
    def widget(self):
        return self._widget

    def toggle_visibility(self, index: int):
        tag_id = self._model.getTag(index)
        visible = self._model.getTagVisibility(tag_id)
        self._model.setTagVisibility(tag_id, not visible)
        self._delegate.render_immediately()
        self._listModel.layoutChanged.emit()
