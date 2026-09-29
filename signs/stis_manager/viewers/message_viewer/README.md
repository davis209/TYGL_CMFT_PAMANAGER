# STIS Message Viewer

# Requirements

Python-3.11, 64bit

```
pip install pyside6
pip install pyinstaller
pip install requests
pip install first
```

# Build Resource File

```
conda activate python311
pyside6-rcc res.qrc -o res.py
```

# Installer

```
pyinstaller -F -w -p .. -i images/main.ico -n STISMessageViewer.exe --version-file file_version_info.txt __main__.py
```
