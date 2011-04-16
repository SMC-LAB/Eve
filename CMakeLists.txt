set (APP_NAME Eve)

project (${APP_NAME})

FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

cmake_minimum_required (VERSION 2.6)

set (EVE_VERSION_MAJOR 0)
set (EVE_VERSION_MINOR 0)
set (EVE_VERSION_PATCH 4)

configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )

set(APP_SOURCES 
  main.cpp
  mainwindow.cpp
  backend.cpp
  transport.cpp
  tagger.cpp
  experiment.cpp
)

set(APP_HEADERS
  mainwindow.h
  backend.h
  transport.h
  tagger.h
  experiment.h
)

set(APP_FORMS
  mainwindow.ui
  transport.ui
  tagger.ui
  experiment.ui
)

qt4_wrap_cpp(APP_HEADERS_MOC ${APP_HEADERS})
qt4_wrap_ui(APP_FORMS_HEADERS ${APP_UI})

add_executable (${APP_NAME} ${APP_SOURCES} ${APP_HEADERS_MOC} ${APP_FORMS_HEADERS})

target_link_libraries (${APP_NAME} marsyas marsyasqt qtcore qtgui qtsql)

