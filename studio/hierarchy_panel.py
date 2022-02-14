from typing import Any, Union, Optional

import PySide6
from PySide6.QtWidgets import QTreeWidget, QTreeWidgetItem
from PySide6.QtCore import QAbstractItemModel, QModelIndex, Qt
from binding_test import RuntimeModel

ModelIndex = Union[PySide6.QtCore.QModelIndex, PySide6.QtCore.QPersistentModelIndex]


class HierarchyPanelController:
    class Delegate:
        def render_immediately(self):
            pass

    def __init__(self, delegate: Delegate, runtime_model: RuntimeModel):
        self.widget = HierarchyView(delegate, runtime_model)


class HierarchyView(QTreeWidget):
    def __init__(self, delegate: HierarchyPanelController.Delegate, runtime_model: RuntimeModel):
        self._delegate = delegate
        super().__init__()
        self._itemMap: dict[int, QTreeWidgetItem] = {}
        self._itemInverse: dict[QTreeWidgetItem, int] = {}
        self._runtime_model = runtime_model

        self.setHeaderLabel("Name")

        root_id = runtime_model.rootObjectId()
        root = self._create_item(root_id, parent=None)
        self.addTopLevelItem(root)

        self.setRootIndex(self.indexFromItem(root))

        self.itemSelectionChanged.connect(self.handle_item_selection)

    def handle_item_selection(self):
        for item in self.selectedItems():
            # TODO: 여러 객체 선택 기능
            oid = self._itemInverse[item]
            self._runtime_model.updateObjectSelectionById(oid)
            break
        self._delegate.render_immediately()

    def _create_item(self, object_id: int, parent: Optional[QTreeWidgetItem]) -> QTreeWidgetItem:
        rt = self._runtime_model
        oid = object_id

        item = QTreeWidgetItem()
        self._itemMap[oid] = item
        self._itemInverse[item] = oid

        name = rt.getObjectName(oid)
        if not name:
            name = f"Object#{oid}"
        item.setText(0, name)

        if parent is not None:
            parent.addChild(item)

        count = rt.getObjectChildrenCount(oid)
        for i in range(count):
            child_id = rt.getObjectChild(oid, i)
            self._create_item(child_id, item)

        return item
