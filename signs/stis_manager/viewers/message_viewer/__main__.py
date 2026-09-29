import sys
import json
import socket
import argparse
from collections import defaultdict
from PySide6.QtCore import (QSortFilterProxyModel, Qt, Slot, QThread, Signal, QObject, QDateTime)
from PySide6.QtGui import (QStandardItemModel, QAction, QIcon, QCursor)
from PySide6.QtWidgets import (QApplication, QMainWindow, QMessageBox, QComboBox, QGridLayout, QMenu,
                               QLabel, QCheckBox, QLineEdit, QDateTimeEdit, QTableView, QAbstractItemView, QVBoxLayout, QWidget, QDockWidget, QSpacerItem, QSizePolicy, QToolBar)

sys.path.append("..")
import res
import config
import common.utils
from common.transactive.location import Location
from common.transactive.pid import Pid
from common.transactive.message import Message
from common.clients.xmlrpcclient import XmlrpcClient
from common.clients.httpclient import HttpClient
from common.clients.localclient import LocalClient
from common.clients.tcpclient import TcpClient

config.host = socket.gethostbyname(config.host)

COLUMN_NAMES = ['Location',
                'Level',
                'PID',
                'Display Message',
                'Tag',
                'Priority',
                'Start Date/Time',
                'End Date/Time',
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

        self.message_label = QLabel("Message")
        self.message_line_edit = QLineEdit()
        self.message_line_edit.setClearButtonEnabled(True)
        self.message_line_edit.textChanged.connect(self._on_changed)

        self.tag_label = QLabel("Tag")
        self.tag_line_edit = QLineEdit()
        self.tag_line_edit.setClearButtonEnabled(True)
        self.tag_line_edit.textChanged.connect(self._on_changed)

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
        self.layout.addWidget(self.message_label, 3, 0)
        self.layout.addWidget(self.tag_label, 4, 0)
        self.layout.addWidget(self.display_template_label, 5, 0)
        self.layout.addWidget(self.start_datetime_label, 6, 0)
        self.layout.addWidget(self.end_datetime_label, 7, 0)

        # column 1
        self.layout.addWidget(self.location_line_edit, 0, 1)
        self.layout.addWidget(self.level_line_edit, 1, 1)
        self.layout.addWidget(self.pid_line_edit, 2, 1)
        self.layout.addWidget(self.message_line_edit, 3, 1)
        self.layout.addWidget(self.tag_line_edit, 4, 1)
        self.layout.addWidget(self.display_template_line_edit, 5, 1)
        self.layout.addWidget(self.start_datetime_edit, 6, 1)
        self.layout.addWidget(self.end_datetime_edit, 7, 1)

        # column 2
        self.layout.addWidget(self.start_datetime_checkbox, 6, 2)
        self.layout.addWidget(self.end_datetime_checkbox, 7, 2)

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

        start_datetime = columns[6]
        if self.start_datetime_filter and isinstance(start_datetime, QDateTime):
            if start_datetime < self.start_datetime_filter:
                return False

        end_datetime = columns[7]
        if self.end_datetime_filter and isinstance(end_datetime, QDateTime):
            if self.end_datetime_filter < end_datetime:
                return False

        return True


class MessageListWidget(QWidget):

    def __init__(self):
        super().__init__()

        self.clear_messages_act = None
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
        self.proxy_view.sortByColumn(11, Qt.AscendingOrder)
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
        self.filters.message_line_edit.textChanged.connect(self._on_filter_changed)
        self.filters.tag_line_edit.textChanged.connect(self._on_filter_changed)
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
            for column in range(len(COLUMN_NAMES)):
                if column != 3:
                    self.proxy_view.resizeColumnToContents(column)
        self.proxy_view.resizeRowsToContents()

    def add_message(self, msg):
        if isinstance(msg, Message):
            model = self.model
            row = model.rowCount()
            model.insertRow(row)
            model.setData(model.index(row, 0), msg.location)
            model.setData(model.index(row, 1), msg.level)
            model.setData(model.index(row, 2), msg.pid)
            model.setData(model.index(row, 3), msg.display_message)
            model.setData(model.index(row, 4), msg.tag)
            model.setData(model.index(row, 5), msg.priority)
            model.setData(model.index(row, 6), common.utils.qt.make_qt_datetime_from_string(msg.start_datetime))
            model.setData(model.index(row, 7), common.utils.qt.make_qt_datetime_from_string(msg.end_datetime))
            model.setData(model.index(row, 8), msg.display_template_type)
            model.setData(model.index(row, 9), msg.id)
            model.setData(model.index(row, 10), common.utils.qt.make_qt_datetime_from_string(msg.template_start_datetime))
            model.setData(model.index(row, 11), common.utils.qt.make_qt_datetime_from_string(msg.template_end_datetime))
        elif isinstance(msg, list):
            for m in msg:
                self.add_message(m)

    def remove_all_rows(self):
        self.model.removeRows(0, self.model.rowCount())

    @Slot()
    def _on_contex_menu(self):
        if self.selection:
            cmenu = QMenu(self)
            cmenu.addAction(self.clear_messages_act)
            cmenu.exec(QCursor.pos())

    @Slot()
    def _on_selection_changed(self):
        self.selection = []
        model = self.proxy_view.model()
        for r in self.proxy_view.selectionModel().selectedRows():
            loc = self.proxy_view.model().data(model.index(r.row(), 0))
            pid = self.proxy_view.model().data(model.index(r.row(), 2))[-3:]
            tag = self.proxy_view.model().data(model.index(r.row(), 4))
            self.selection.append((loc, pid, tag))

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
        filter_map[3] = self.filters.message_line_edit.text()
        filter_map[4] = self.filters.tag_line_edit.text()
        filter_map[8] = self.filters.display_template_line_edit.text()

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
    messages: MessageListWidget = None
    _refresh_messages_act: QAction = None
    _clear_messages_act: QAction = None
    _exit_act: QAction = None
    _about_act: QAction = None
    _show_hide_toolbar_act: QAction = None
    _show_hide_filters_act: QAction = None
    tool_bar: QToolBar = None
    dock_filters: QDockWidget = None
    location_selection: LocationComboBox = None
    status_message: QLabel = None
    status_timestamp: QLabel = None
    status_server: QLabel = None

    def __init__(self):
        super().__init__()
        self.client = self.create_client()

        self.messages = MessageListWidget()
        self.messages.setEnabled(False)
        self.setCentralWidget(self.messages)

        self.dock_filters = QDockWidget('Filters', self)
        self.dock_filters.setWidget(self.messages.filters)
        self.addDockWidget(Qt.RightDockWidgetArea, self.dock_filters)
        self.dock_filters.dockLocationChanged.connect(self.messages.filters.on_dock_location_changed)

        self.create_actions()
        self.create_tool_bar()
        self.create_status_bar()
        self.create_menus()

        self.messages.clear_messages_act = self._clear_messages_act
        self.messages.proxy_view.selectionModel().selectionChanged.connect(self._on_messages_selection_changed)

        self.setWindowTitle("STIS Message Viewer")
        self.setWindowIcon(QIcon(':/images/main.ico'))

        left = config.left or QCursor.pos().x() // config.width * config.width
        self.move(left, config.top)
        self.resize(config.width, config.height - 32)  # TODO: fix the hardcode 32

        class Worker(QThread):

            def __init__(self, parent):
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
        Location.set_this(config.location)

        pids, error = self.client.get_pids()
        if error:
            self.status_message.setText(error)
            return error

        Pid.set(pids)
        self.status_message.setText('Ready')

    @Slot()
    def on_startup(self):
        if Location.this().is_occ():
            self.location_selection.combobox.addItems(['ALL'])
        self.location_selection.combobox.addItems(Location.station_and_depot_display_names())

        if not Location.this().is_occ():
            index = self.location_selection.combobox.findText(Location.this().display_name)
            self.location_selection.combobox.setCurrentIndex(index)
            self.location_selection.setDisabled(True)
            self.messages.proxy_view.setColumnHidden(0, True)
            self.messages.filters.location_label.setVisible(False)
            self.messages.filters.location_line_edit.setVisible(False)
        else:
            self.location_selection.combobox.currentIndexChanged.connect(self._on_location_selection_combox_index_changed)

        self.refresh_messages_async()

    def create_client(self):
        match config.server_type:
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
        self._refresh_messages_act = QAction(QIcon(':/images/refresh.png'), "&Refresh", self, shortcut="Ctrl+R", statusTip="Refresh messages", triggered=self.refresh_messages_async)
        self._clear_messages_act = QAction(QIcon(':/images/clear.png'), "&Clear", self, shortcut="Del", statusTip="Clear messages", triggered=self.clear_messages, enabled=False)
        self._exit_act = QAction("E&xit", self, shortcut="Ctrl+Q", statusTip="Exit the application", triggered=self.close)
        self._about_act = QAction("&About", self, statusTip="Show the application's About box", triggered=self.about)
        self._show_hide_toolbar_act = QAction("Show/Hide &Toolbar", self, statusTip="Show/Hide the application's Toobar", checkable=True, checked=True, triggered=self.show_hide_tool_bar)
        self._show_hide_filters_act = QAction("Show/Hide &Filters", self, statusTip="Show/Hide the application's Filters", checkable=True, checked=True, triggered=self.show_hide_filters)

    def create_menus(self):
        file_menu = self.menuBar().addMenu("&File")
        file_menu.addAction(self._exit_act)
        view_menu = self.menuBar().addMenu("&View")
        view_menu.addActions(self.createPopupMenu().actions())
        help_menu = self.menuBar().addMenu("&Help")
        help_menu.addAction(self._about_act)

    def create_tool_bar(self):
        self.tool_bar = self.addToolBar("Toolbar")
        self.tool_bar.setToolButtonStyle(Qt.ToolButtonTextUnderIcon)
        self.tool_bar.addAction(self._refresh_messages_act)
        self.tool_bar.addAction(self._clear_messages_act)
        self.location_selection = LocationComboBox()
        self.tool_bar.addWidget(self.location_selection)

    def create_status_bar(self):
        self.status_message = QLabel('Ready')
        self.status_server = QLabel(f'{config.server_type.upper()} {config.host}:{config.port}')
        self.status_timestamp = QLabel('Last update:')
        self.statusBar().addWidget(self.status_message, stretch=100)
        self.statusBar().addWidget(self.status_timestamp)
        # self.statusBar().addWidget(self.status_server)

    @Slot()
    def refresh_messages_sync(self, messages=None, error=None):
        location = self.location_selection.combobox.currentText().strip() or 'ALL'
        messages, error = self.client.get_messages(location)

        if not error:
            self.messages.setEnabled(True)
            self.messages.remove_all_rows()
            self.messages.add_message(messages)
            self.messages.resize_columns_rows_to_contents()
        else:
            self.messages.proxy_view.clearSelection()
            self.messages.setEnabled(False)
            self._clear_messages_act.setEnabled(False)

        self.status_message.setText(error or "Ready")

    @Slot()
    def refresh_messages_async(self):
        self._refresh_messages_act.setDisabled(True)
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
                messages, error = self.main.client.get_messages(location)
                self.signals.done.emit(messages, error)

        worker = Worker(self)
        worker.signals.done.connect(self.set_messages)
        worker.start()

    @Slot()
    def set_messages(self, messages, error):
        if not error:
            self.messages.setEnabled(True)
            self.messages.remove_all_rows()
            self.messages.add_message(messages)
            self.messages.resize_columns_rows_to_contents()
            self.status_timestamp.setText('Last update: ' + QDateTime.currentDateTime().toString('yyyy/MM/dd HH:MM:ss'))
        else:
            self.messages.proxy_view.clearSelection()
            self.messages.setEnabled(False)
            self._clear_messages_act.setEnabled(False)
        self.status_message.setText(error or "Ready")
        self._refresh_messages_act.setDisabled(False)

    @Slot()
    def clear_messages(self):
        error = self.client.clear_messages(json.dumps(self.messages.selection))
        if error:
            self.status_message.setText(error)
        elif config.auto_refresh_after_clear:
            self.refresh_messages_async()

    @Slot()
    def _on_messages_selection_changed(self):
        self._clear_messages_act.setEnabled(0 < len(self.messages.selection))
        status_map = defaultdict(set[str])
        for loc, pid, _ in self.messages.selection:
            status_map[loc].add(pid)
        status = ', '.join([loc + '[' + ','.join(pids) + ']' for loc, pids in status_map.items()])
        self.status_message.setText(status or 'Ready')

    @Slot()
    def _on_location_selection_combox_index_changed(self, index):
        self.refresh_messages_async()

    @Slot()
    def about(self):
        content = f'Component: {config.component}\nVersion: {config.version}\nBuild Date: {config.build_data}\n{config.copyright}'
        QMessageBox.about(self, "About STIS Message Viewer", content)

    @Slot()
    def show_hide_tool_bar(self):
        # already toggled
        is_checked = self._show_hide_toolbar_act.isChecked()
        self.tool_bar.setVisible(is_checked)

    @Slot()
    def show_hide_filters(self):
        is_checked = self._show_hide_filters_act.isChecked()
        self.dock_filters.setVisible(is_checked)


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--location')
    parser.add_argument('--host')
    parser.add_argument('--port', type=int)
    parser.add_argument('--timeout', type=int)
    parser.add_argument('--server-type')
    parser.add_argument('--auto-refresh-after-clear', action='store_true')
    args, _ = parser.parse_known_args()
    config.location = args.location or config.location
    config.host = args.host or config.host
    config.host = socket.gethostbyname(config.host)
    config.port = args.port or config.port
    config.timeout = args.timeout or config.timeout
    config.server_type = args.server_type or config.server_type
    config.auto_refresh_after_clear = args.auto_refresh_after_clear or config.auto_refresh_after_clear


if __name__ == '__main__':
    parse_args()
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())
