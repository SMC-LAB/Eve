QT       += core gui sql

TARGET   = Eve
TEMPLATE = app

SOURCES += main.cpp       \
           mainwindow.cpp \
           backend.cpp    \
           transport.cpp  \
           tagger.cpp     \
           experiment.cpp

HEADERS += mainwindow.h   \
           backend.h      \
           transport.h    \
           tagger.h       \
           experiment.h

FORMS   += mainwindow.ui  \
           transport.ui   \
           tagger.ui      \
           experiment.ui

MARSYAS_INSTALL_DIR = /usr/local/marsyas-static
QT_INSTALL_DIR = /usr/local/qt-static

INCLUDEPATH = $$QT_INSTALL_DIR/include/QtCore         \
              $$QT_INSTALL_DIR/include/QtGui          \
              $$QT_INSTALL_DIR/include/QtSql          \
              $$MARSYAS_INSTALL_DIR/include/marsyas   \
              $$MARSYAS_INSTALL_DIR/include/marsyasqt

QMAKE_LIBDIR += $$MARSYAS_INSTALL_DIR/lib $$QT_INSTALL_DIR/lib
QMAKE_LFLAGS += -static

win32-msvc2005:LIBS += marsyas.lib marsyasqt.lib
unix:LIBS           += -lmarsyas -lmarsyasqt
macx:LIBS           += -framework CoreAudio -framework CoreMidi -framework CoreFoundation
unix:!macx:LIBS += -lasound -lmp3lame -logg -lvorbisfile

