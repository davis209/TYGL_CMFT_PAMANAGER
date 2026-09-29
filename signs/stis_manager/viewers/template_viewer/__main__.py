import sys
import json
import socket
import argparse
from collections import defaultdict
from collections import namedtuple
from PySide6.QtCore import (QSortFilterProxyModel, Qt, Slot, QThread, Signal, QObject, QDateTime)
from PySide6.QtGui import QStandardItemModel, QAction, QIcon, QCursor
from PySide6.QtWidgets import (QApplication, QMainWindow, QMessageBox, QComboBox, QGridLayout, QMenu,
                               QLabel, QCheckBox, QLineEdit, QDateEdit, QTimeEdit, QDateTimeEdit, QTableView, QAbstractItemView, QVBoxLayout, QWidget, QDockWidget, QSpacerItem, QSizePolicy)

sys.path.append("..")
import res
import config
import common.config
import common.utils
from common.transactive.location import Location
from common.transactive.pid import Pid
from common.transactive.template import Template
from common.transactive.template import RemoveTemplateArg
from common.transactive.template import display_template_type_to_number, display_template_type_to_str
from common.transactive.template import PredefinedDisplayTemplate
from common.clients.xmlrpcclient import XmlrpcClient
from common.clients.httpclient_v1 import HttpClient
from common.clients.localclient import LocalClient
from common.clients.tcpclient import TcpClient

common.config.host = socket.gethostbyname(common.config.host)

COLUMN_NAMES = ['Location',
                'Level',
                'PID',
                'Display Template',
                'ID',
                'Start Date/Time',
                'End Date/Time']


class FiltersWidget(QWidget):

    def __init__(self, parent=None):
        super().__init__(parent)

        self.location_label = QLabel("Location")
        self.location_line_edit = QLineEdit()
        self.location_line_edit.setClearButtonEnabled(True)
        self.location_line_edit.textChanged.connect(self._on_changed)

        self.level_label = QLabel("Level")
        self.level_line_edit = QLineEdit()
        self.level_line_edit.setClearButtonEnabled(True)
        self.level_line_edit.textChanged.connect(self._on_changed)

        self.pid_label = QLabel("PID")
        self.pid_line_edit = QLineEdit()
        self.pid_line_edit.setClearButtonEnabled(True)
        self.pid_line_edit.textChanged.connect(self._on_changed)

        self.display_template_label = QLabel("Template")
        self.display_template_line_edit = QLineEdit()
        self.display_template_line_edit.setClearButtonEnabled(True)
        self.display_template_line_edit.textChanged.connect(self._on_changed)

        self.start_datetime_label = QLabel("Start Date/Time")
        self.start_datetime_edit = QDateTimeEdit(QDateTime.currentDateTime())
        self.start_datetime_edit.dateTimeChanged.connect(self._on_changed)
        self.start_datetime_edit.setEnabled(False)
        self.start_datetime_checkbox = QCheckBox()
        self.start_datetime_checkbox.stateChanged.connect(self._on_changed)

        self.end_datetime_label = QLabel("End Date/Time")
        self.end_datetime_edit = QDateTimeEdit(QDateTime.currentDateTime())
        self.end_datetime_edit.dateTimeChanged.connect(self._on_changed)
        self.end_datetime_edit.setEnabled(False)
        self.end_datetime_checkbox = QCheckBox()
        self.end_datetime_checkbox.stateChanged.connect(self._on_changed)

        self.layout = QGridLayout()

        # column 0
        self.layout.addWidget(self.location_label, 0, 0)
        self.layout.addWidget(self.level_label, 1, 0)
        self.layout.addWidget(self.pid_label, 2, 0)
        self.layout.addWidget(self.display_template_label, 3, 0)
        self.layout.addWidget(self.start_datetime_label, 4, 0)
        self.layout.addWidget(self.end_datetime_label, 5, 0)

        # column 1
        self.layout.addWidget(self.location_line_edit, 0, 1)
        self.layout.addWidget(self.level_line_edit, 1, 1)
        self.layout.addWidget(self.pid_line_edit, 2, 1)
        self.layout.addWidget(self.display_template_line_edit, 3, 1)
        self.layout.addWidget(self.start_datetime_edit, 4, 1)
        self.layout.addWidget(self.end_datetime_edit, 5, 1)

        # column 2
        self.layout.addWidget(self.start_datetime_checkbox, 4, 2)
        self.layout.addWidget(self.end_datetime_checkbox, 5, 2)

        self.layout.addItem(QSpacerItem(0, 0, QSizePolicy.Minimum, QSizePolicy.Expanding))
        self.setLayout(self.layout)

    @Slot()
    def _on_changed(self):
        self.start_datetime_edit.setEnabled(self.start_datetime_checkbox.checkState() == Qt.CheckState.Checked)
        self.end_datetime_edit.setEnabled(self.end_datetime_checkbox.checkState() == Qt.CheckState.Checked)

    @Slot()
    def on_dock_location_changed(self, area: Qt.DockWidgetArea):
        pass


class MultiColumnSortFilterProxyModel(QSortFilterProxyModel):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.filter_map = {}
        self.key_column = -1
        self.start_datetime_filter = None
        self.end_datetime_filter = None

    def setFilterKeyColumn(self, column: int) -> None:
        self.key_column = column
        super().setFilterKeyColumn(column)

    def filterAcceptsRow(self, source_row: int, source_parent):
        if self.key_column != -1:
            return super().filterAcceptsRow(source_row, source_parent)

        sm = self.sourceModel()

        columns = []
        for i in range(sm.columnCount(source_parent)):
            index = sm.index(source_row, i, source_parent)
            columns.append(sm.data(index))

        for k, v in self.filter_map.items():
            v = v.upper()
            column = columns[int(k)]
            if isinstance(column, str):
                if v not in column.upper():
                    return False

        start_datetime = columns[5]
        if self.start_datetime_filter and isinstance(start_datetime, QDateTime):
            if start_datetime < self.start_datetime_filter:
                return False

        end_datetime = columns[6]
        if self.end_datetime_filter and isinstance(end_datetime, QDateTime):
            if self.end_datetime_filter < end_datetime:
                return False

        return True


class TemplateListWidget(QWidget):

    def __init__(self):
        super().__init__()

        self.remove_templates_act = None
        self.selection = None

        self.model = QStandardItemModel(0, len(COLUMN_NAMES), self)

        for i, column in enumerate(COLUMN_NAMES):
            self.model.setHeaderData(i, Qt.Horizontal, column)

        self.proxy_model = MultiColumnSortFilterProxyModel()
        self.proxy_model.setSourceModel(self.model)
        self.proxy_model.setDynamicSortFilter(True)
        self.proxy_model.setSortCaseSensitivity(Qt.CaseInsensitive)

        self.proxy_view = QTableView()
        self.proxy_view.setAlternatingRowColors(True)
        self.proxy_view.setModel(self.proxy_model)
        self.proxy_view.setSortingEnabled(True)
        self.proxy_view.setSelectionBehavior(QAbstractItemView.SelectionBehavior.SelectRows)
        # self.proxy_view.setColumnHidden(11, True)
        # self.proxy_view.sortByColumn(11, Qt.AscendingOrder)
        self.proxy_view.setContextMenuPolicy(Qt.CustomContextMenu)
        self.proxy_view.customContextMenuRequested.connect(self._on_contex_menu)
        self.proxy_view.selectionModel().selectionChanged.connect(self._on_selection_changed)
        self.proxy_view.horizontalHeader().sectionResized.connect(self._on_column_resized)
        self.proxy_view.setWordWrap(True)

        for i, width in enumerate(config.column_width):
            self.proxy_view.setColumnWidth(i, width)

        self.filters = FiltersWidget(self)
        self.filters.location_line_edit.textChanged.connect(self._on_filter_changed)
        self.filters.level_line_edit.textChanged.connect(self._on_filter_changed)
        self.filters.pid_line_edit.textChanged.connect(self._on_filter_changed)
        self.filters.display_template_line_edit.textChanged.connect(self._on_filter_changed)
        self.filters.start_datetime_edit.dateTimeChanged.connect(self._on_filter_changed)
        self.filters.end_datetime_edit.dateTimeChanged.connect(self._on_filter_changed)
        self.filters.start_datetime_checkbox.stateChanged.connect(self._on_filter_changed)
        self.filters.end_datetime_checkbox.stateChanged.connect(self._on_filter_changed)

        main_layout = QVBoxLayout()
        main_layout.addWidget(self.proxy_view)
        self.setLayout(main_layout)

    def resize_columns_rows_to_contents(self):
        if config.auto_resize_columns:
            self.proxy_view.resizeColumnsToContents()
        self.proxy_view.resizeRowsToContents()

    def add_templates(self, tpl):
        # if isinstance(tpl, Template):
        if isinstance(tpl, Template):
            model = self.model
            row = model.rowCount()
            model.insertRow(row)
            model.setData(model.index(row, 0), tpl.location)
            model.setData(model.index(row, 1), tpl.level)
            model.setData(model.index(row, 2), tpl.pid)
            model.setData(model.index(row, 3), tpl.display_template_type)
            model.setData(model.index(row, 4), tpl.id)
            model.setData(model.index(row, 5), common.utils.qt.make_qt_datetime_from_string(tpl.template_start_datetime))
            model.setData(model.index(row, 6), common.utils.qt.make_qt_datetime_from_string(tpl.template_end_datetime))
        elif isinstance(tpl, list):
            for m in tpl:
                self.add_templates(m)

    def remove_all_rows(self):
        self.model.removeRows(0, self.model.rowCount())

    @Slot()
    def _on_contex_menu(self):
        if self.selection:
            cmenu = QMenu(self)
            cmenu.addAction(self.remove_templates_act)
            cmenu.exec(QCursor.pos())

    @Slot()
    def _on_selection_changed(self):
        self.selection = []
        model = self.proxy_view.model()
        for r in self.proxy_view.selectionModel().selectedRows():
            location = self.proxy_view.model().data(model.index(r.row(), 0))
            pid = self.proxy_view.model().data(model.index(r.row(), 2))[-3:]

            template_type = self.proxy_view.model().data(model.index(r.row(), 3))
            template_id = self.proxy_view.model().data(model.index(r.row(), 4))
            template_start_time = self.proxy_view.model().data(model.index(r.row(), 5))
            template_end_time = self.proxy_view.model().data(model.index(r.row(), 6))

            pdt = PredefinedDisplayTemplate()
            pdt.display_template_type = int(display_template_type_to_number(template_type))
            pdt.display_template_id = template_id
            pdt.start_time = common.utils.qt.datetime_to_string(template_start_time)
            pdt.end_time = common.utils.qt.datetime_to_string(template_end_time)

            arg = RemoveTemplateArg()
            arg.location = location
            arg.pids.append(pid)
            arg.display_template_list.append(PredefinedDisplayTemplate.to_json(pdt))

            self.selection.append(arg)

    @Slot()
    def _on_column_resized(self, logicalIndex, oldSize, newSize):
        if logicalIndex == 3:
            self.resize_columns_rows_to_contents()

    @Slot()
    def _on_filter_changed(self):
        filter_map = self.proxy_model.filter_map
        filter_map[0] = self.filters.location_line_edit.text()
        filter_map[1] = self.filters.level_line_edit.text()
        filter_map[2] = self.filters.pid_line_edit.text()
        filter_map[3] = self.filters.display_template_line_edit.text()

        self.proxy_model.start_datetime_filter = self.filters.start_datetime_edit.dateTime() if self.filters.start_datetime_edit.isEnabled() else None
        self.proxy_model.end_datetime_filter = self.filters.end_datetime_edit.dateTime() if self.filters.end_datetime_edit.isEnabled() else None

        self.proxy_model.setFilterKeyColumn(-1)
        self.resize_columns_rows_to_contents()


class LocationComboBox(QWidget):
    def __init__(self):
        super().__init__()
        self.combobox = QComboBox()
        self.combobox.setSizeAdjustPolicy(QComboBox.SizeAdjustPolicy.AdjustToContents)
        self.label = QLabel('Location')
        layout = QVBoxLayout()
        layout.addWidget(self.label)
        layout.addWidget(self.combobox)
        self.setLayout(layout)

    def init(self):
        pass


class MainWindow(QMainWindow):
    templates: TemplateListWidget = None
    _refresh_templates_act: QAction = None
    _remove_templates_act: QAction = None
    _exit_act: QAction = None
    _about_act: QAction = None
    location_selection: LocationComboBox = None
    status_message: QLabel = None
    status_timestamp: QLabel = None
    status_server: QLabel = None

    def __init__(self):
        super().__init__()
        self.client = self.create_client()

        self.templates = TemplateListWidget()
        self.templates.setEnabled(False)
        self.setCentralWidget(self.templates)

        dock = QDockWidget('Filters', self)
        dock.setWidget(self.templates.filters)
        self.addDockWidget(Qt.RightDockWidgetArea, dock)
        dock.dockLocationChanged.connect(self.templates.filters.on_dock_location_changed)

        self.create_actions()
        self.create_tool_bar()
        self.create_status_bar()
        self.create_menus()

        self.templates.remove_templates_act = self._remove_templates_act
        self.templates.proxy_view.selectionModel().selectionChanged.connect(self._on_templates_selection_changed)

        self.setWindowTitle("STIS Template Viewer")
        self.setWindowIcon(QIcon(':/images/main.ico'))

        left = config.left or QCursor.pos().x() // config.width * config.width
        self.move(left, config.top)
        self.resize(config.width, config.height - 32)  # TODO: fix the hardcode 32

        class Worker(QThread):

            def __init__(self, parent=None):
                super().__init__(parent)
                self.main = parent

                class Signals(QObject):
                    done = Signal()

                self.signals = Signals()

            def run(self):
                error = self.main.init()
                if not error:
                    self.signals.done.emit()

        worker = Worker(self)
        worker.signals.done.connect(self.on_startup)
        worker.start()

    def init(self):
        self.status_message.setText('Connecting to STISManager...')
        locations, error = self.client.get_locations()
        if error:
            self.status_message.setText(error)
            return error

        Location.set(locations)

        pids, error = self.client.get_pids()
        if error:
            self.status_message.setText(error)
            return error

        Pid.set(pids)

    @Slot()
    def on_startup(self):
        if Location.this().is_occ():
            self.location_selection.combobox.addItems(['ALL'])
        self.location_selection.combobox.addItems(Location.station_and_depot_display_names())

        if not Location.this().is_occ():
            index = self.location_selection.combobox.findText(Location.this().display_name)
            self.location_selection.combobox.setCurrentIndex(index)
            self.location_selection.setDisabled(True)
            self.templates.proxy_view.setColumnHidden(0, True)
            self.templates.filters.location_label.setVisible(False)
            self.templates.filters.location_line_edit.setVisible(False)
        else:
            self.location_selection.combobox.currentIndexChanged.connect(self._on_location_selection_combox_index_changed)

        self.refresh_templates_async()

    def create_client(self):
        match common.config.server_type:
            case 'xmlrpc':
                return XmlrpcClient()
            case 'http':
                return HttpClient()
            case 'tcp':
                return TcpClient()
            case 'local':
                return LocalClient()
            case _:
                return LocalClient()

    def create_actions(self):
        self._refresh_templates_act = QAction(QIcon(':/images/refresh.png'), "&Refresh", self, shortcut="Ctrl+R", statusTip="Refresh templates", triggered=self.refresh_templates_async)
        self._remove_templates_act = QAction(QIcon(':/images/clear.png'), "&Clear", self, shortcut="Del", statusTip="Clear templates", triggered=self.remove_templates, enabled=False)
        self._exit_act = QAction("E&xit", self, shortcut="Ctrl+Q", statusTip="Exit the application", triggered=self.close)
        self._about_act = QAction("&About", self, statusTip="Show the application's About box", triggered=self.about)

    def create_menus(self):
        file_menu = self.menuBar().addMenu("&File")
        file_menu.addAction(self._exit_act)
        view_menu = self.menuBar().addMenu("&View")
        view_menu.addActions(self.createPopupMenu().actions())
        help_menu = self.menuBar().addMenu("&Help")
        help_menu.addAction(self._about_act)

    def create_tool_bar(self):
        tool_bar = self.addToolBar("Toolbar")
        tool_bar.setToolButtonStyle(Qt.ToolButtonTextUnderIcon)
        tool_bar.addAction(self._refresh_templates_act)
        tool_bar.addAction(self._remove_templates_act)
        self.location_selection = LocationComboBox()
        tool_bar.addWidget(self.location_selection)

    def create_status_bar(self):
        self.status_message = QLabel('Ready')
        self.status_timestamp = QLabel('Last Update:')
        self.status_server = QLabel(f'{common.config.server_type.upper()} {common.config.host}:{common.config.port}')
        self.statusBar().addWidget(self.status_message, stretch=100)
        self.statusBar().addWidget(self.status_timestamp)
        # self.statusBar().addWidget(self.status_server)

    @Slot()
    def refresh_templates_sync(self):
        location = self.location_selection.combobox.currentText().strip() or 'ALL'
        templates, error = self.client.get_templates(location)
        if not error:
            self.templates.setEnabled(True)
            self.templates.remove_all_rows()
            self.templates.add_templates(templates)
            self.templates.resize_columns_rows_to_contents()
        else:
            self.templates.proxy_view.clearSelection()
            self.templates.setEnabled(False)
            self._remove_templates_act.setEnabled(False)
        self.status_message.setText(error or "Ready")

    @Slot()
    def refresh_templates_async(self):
        self._refresh_templates_act.setDisabled(True)
        self.status_message.setText("Refreshing...")

        class Worker(QThread):

            def __init__(self, parent):
                super().__init__(parent)
                self.main = parent

                class Signals(QObject):
                    done = Signal(object, object)

                self.signals = Signals()

            def run(self):
                location = self.main.location_selection.combobox.currentText().strip() or 'ALL'
                templates, error = self.main.client.get_templates(location)
                self.signals.done.emit(templates, error)

        worker = Worker(self)
        worker.signals.done.connect(self.set_templates)
        worker.start()

    @Slot()
    def set_templates(self, templates=None, error=None):
        if not error:
            self.templates.setEnabled(True)
            self.templates.remove_all_rows()
            self.templates.add_templates(templates)
            self.templates.resize_columns_rows_to_contents()
            self.status_timestamp.setText('Last update: ' + QDateTime.currentDateTime().toString('yyyy/MM/dd HH:MM:ss'))
        else:
            self.templates.proxy_view.clearSelection()
            self.templates.setEnabled(False)
            self._remove_templates_act.setEnabled(False)
        self.status_message.setText(error or "Ready")
        self._refresh_templates_act.setDisabled(False)

    @Slot()
    def remove_templates(self):
        error = self.client.remove_templates(RemoveTemplateArg.to_json(self.templates.selection))
        # error = self.client.remove_templates(json.dumps(self.templates.selection))
        if error:
            self.status_message.setText(error)
        elif config.auto_refresh_after_clear:
            self.refresh_templates_async()

    @Slot()
    def _on_templates_selection_changed(self):
        self._remove_templates_act.setEnabled(0 < len(self.templates.selection))
        # self.statusBar().showMessage(','.join(self.templates.selection.keys()) or 'Ready')
        locations = []
        for arg in self.templates.selection:
            location = arg.location.strip()
            if location not in locations:
                locations.append(location)
        status = ','.join(locations)
        self.status_message.setText(status or 'Ready')

    @Slot()
    def _on_location_selection_combox_index_changed(self, index):
        self.refresh_templates_async()

    @Slot()
    def about(self):
        content = f'Component: {config.component}\nVersion: {config.version}\nBuild Date: {config.build_data}\n{config.copyright}'
        QMessageBox.about(self, "About STIS Template Viewer", content)


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--location')
    parser.add_argument('--host')
    parser.add_argument('--port', type=int)
    parser.add_argument('--timeout', type=int)
    parser.add_argument('--server-type')
    parser.add_argument('--auto-refresh-after-clear', action='store_true')
    args, _ = parser.parse_known_args()
    common.config.location = args.location or common.config.location
    common.config.host = args.host or common.config.host
    common.config.host = socket.gethostbyname(common.config.host)
    common.config.port = args.port or common.config.port
    common.config.timeout = args.timeout or common.config.timeout
    common.config.server_type = args.server_type or common.config.server_type
    config.auto_refresh_after_clear = args.auto_refresh_after_clear or config.auto_refresh_after_clear


if __name__ == '__main__':
    parse_args()
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())
