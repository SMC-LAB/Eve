set (APP_NAME Eve)

project (${APP_NAME})

FIND_PACKAGE(Qt4 REQUIRED)
SET(QT_USE_QTSQL TRUE)
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

find_path(MARSYAS_INCLUDE_DIR MarSystem.h PATH_SUFFIXES marsyas)
find_path(MARSYASQT_INCLUDE_DIR MarSystemQtWrapper.h PATH_SUFFIXES marsyasqt)
find_library(MARSYAS_LIBRARY NAMES marsyas)
find_library(MARSYASQT_LIBRARY NAMES marsyasqt)

if (MARSYAS_INCLUDE_DIR AND MARSYAS_LIBRARY AND MARSYASQT_LIBRARY)
	set(MARSYAS_FOUND TRUE)
endif (MARSYAS_INCLUDE_DIR AND MARSYAS_LIBRARY AND MARSYASQT_LIBRARY)

if (MARSYAS_FOUND)
	if (NOT MARSYAS_FIND_QUIETLY)
		message (STATUS "Found MARSYAS: ${MARSYAS_LIBRARY}")
		message (STATUS "Found MARSYAS: ${MARSYASQT_LIBRARY}")
	endif (NOT MARSYAS_FIND_QUIETLY)
else (MARSYAS_FOUND)
	if (MARSYAS_FIND_REQUIRED)
		message (FATAL_ERROR "Could find: MARSYAS")
	endif (MARSYAS_FIND_REQUIRED)
endif (MARSYAS_FOUND)

include_directories(${MARSYAS_INCLUDE_DIR} ${MARSYASQT_INCLUDE_DIR} ${PROJECT_BINARY_DIR})

cmake_minimum_required (VERSION 2.6)

set (EVE_VERSION_MAJOR 0)
set (EVE_VERSION_MINOR 0)
set (EVE_VERSION_PATCH 4)

configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
)

set(APP_SOURCES 
  src/main.cpp
  src/mainwindow.cpp
  src/backend.cpp
  src/transport.cpp
  src/tagger.cpp
  src/experiment.cpp
)

set(APP_HEADERS
  src/mainwindow.h
  #src/backend.h
  src/transport.h
  src/tagger.h
  src/experiment.h
)

set(APP_FORMS
  src/mainwindow.ui
  src/transport.ui
  src/tagger.ui
  src/experiment.ui
)

qt4_wrap_cpp(APP_HEADERS_MOC ${APP_HEADERS})
qt4_wrap_ui(APP_FORMS_HEADERS ${APP_FORMS})

add_executable (${APP_NAME} ${APP_SOURCES} ${APP_HEADERS_MOC} ${APP_FORMS_HEADERS})

target_link_libraries (${APP_NAME} marsyas marsyasqt QtCore QtGui QtSql)
