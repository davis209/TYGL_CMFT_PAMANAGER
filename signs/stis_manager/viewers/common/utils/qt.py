from PySide6.QtCore import (QDateTime)


def make_qt_datetime_from_string(text: str):
    return QDateTime.fromString(text, 'yyyyMMddhhmmss') if text else ''


def datetime_to_string(t: QDateTime):
    return t.toString('yyyyMMddhhmmss')
