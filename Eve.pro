TEMPLATE = app

QT       += core gui sql

TARGET   = Eve

SOURCES += src/main.cpp       \
           src/mainwindow.cpp \
           src/backend.cpp    \
           src/transport.cpp  \
           src/tagger.cpp     \
           src/experiment.cpp

HEADERS += src/mainwindow.h   \
           src/backend.h      \
           src/transport.h    \
           src/tagger.h       \
           src/experiment.h

FORMS   += src/mainwindow.ui  \
           src/transport.ui   \
           src/tagger.ui      \
           src/experiment.ui

MARSYAS_INSTALL_DIR = /usr/local

INCLUDEPATH += $$MARSYAS_INSTALL_DIR/include/marsyas/  \
               $$MARSYAS_INSTALL_DIR/include/marsyasqt

QMAKE_DIR   += $$MARSYAS_INSTALL_DIR/lib/

win32-msvc2005:LIBS += marsyas.lib marsyasqt.lib
unix:LIBS           += -lmarsyas -lmarsyasqt -L$$QMAKE_DIR
macx:LIBS           += -framework CoreAudio -framework CoreMidi -framework CoreFoundation
unix:!macx:LIBS     += -lasound -lmp3lame -lmad -logg -lvorbisfile
