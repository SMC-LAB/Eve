###  your files
SOURCES = main.cpp
HEADERS = mainwindow.h
SOURCES += mainwindow.cpp
HEADERS += backend.h
SOURCES += backend.cpp

MARSYAS_INSTALL_DIR = /usr/local
INCLUDEPATH += $$MARSYAS_INSTALL_DIR/include/marsyas/
INCLUDEPATH += $$MARSYAS_INSTALL_DIR/include/marsyasqt
QMAKE_DIR += $$MARSYAS_INSTALL_DIR/lib/

### basic OS stuff; do not change!
win32-msvc2005:LIBS += marsyas.lib marsyasqt.lib
unix:LIBS += -lmarsyas -lmarsyasqt -L$$QMAKE_DIR
macx:LIBS += -framework CoreAudio -framework CoreMidi -framework CoreFoundation
unix:!macx:LIBS += -lasound
