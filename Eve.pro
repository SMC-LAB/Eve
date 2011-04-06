QT       += core gui #sql

TARGET   = Eve
TEMPLATE = app

SOURCES += main.cpp       \
           mainwindow.cpp \
           backend.cpp

HEADERS += mainwindow.h   \
           backend.h

FORMS   += mainwindow.ui

MARSYAS_INSTALL_DIR = /usr/local

INCLUDEPATH += $$MARSYAS_INSTALL_DIR/include/marsyas/  \
               $$MARSYAS_INSTALL_DIR/include/marsyasqt

QMAKE_DIR   += $$MARSYAS_INSTALL_DIR/lib/

win32-msvc2005:LIBS += marsyas.lib marsyasqt.lib
unix:LIBS           += -lmarsyas -lmarsyasqt -L$$QMAKE_DIR
macx:LIBS           += -framework CoreAudio -framework CoreMidi -framework CoreFoundation
unix:!macx:LIBS += -lasound
