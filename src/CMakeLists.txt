set (APP_NAME Eve)

set (EVE_VERSION_MAJOR 0)
set (EVE_VERSION_MINOR 0)
set (EVE_VERSION_PATCH 4)

project (${APP_NAME})

IF (APPLE)
  find_library(COREAUDIO_LIBRARY CoreAudio)
  find_library(COCOA_LIBRARY Cocoa)
  set(EXTRA_LIBS ${COREAUDIO_LIBRARY} ${COCOA_LIBRARY})
  set(CMAKE_FIND_FRAMEWORK "LAST")
ELSEIF (UNIX)
  find_library(ASOUND_LIBRARY asound)
  set(EXTRA_LIBS ${ASOUND_LIBRARY})
ELSEIF (WIN32)
  find_library(DSOUND_LIBRARY dsound)
  set(EXTRA_LIBS ${DSOUND_LIBRARY})
ELSE (APPLE)
  message(FATAL_ERROR "Unknown architecture")
ENDIF (APPLE)

FIND_PACKAGE(Qt4 REQUIRED QtCore QtGui QtSql)
SET(QT_USE_QTSQL TRUE)
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

option (EVE_STATIC "Compilation will statically link to Marsyas" ON)

if (EVE_STATIC)
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".a;.lib;.so;.dll;.dylib")
  add_library(marsyas STATIC IMPORTED)
  add_library(marsyasqt STATIC IMPORTED)
  add_library(qsqlite STATIC IMPORTED)
  message(STATUS "Compilation will statically link to Marsyas")
else (EVE_STATIC)
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".so;.dll;.dylib;.a;.lib")
endif(EVE_STATIC)

set(MARSYAS_LIBRARY MARSYAS_LIBRARY-NOTFOUND)
set(MARSYASQT_LIBRARY MARSYASQT_LIBRARY-NOTFOUND)
set(QSQLITE_LIBRARY QSQLITE_LIBRARY-NOTFOUND)

find_path(MARSYAS_INCLUDE_DIR MarSystem.h PATH_SUFFIXES marsyas)
find_path(MARSYASQT_INCLUDE_DIR MarSystemQtWrapper.h PATH_SUFFIXES marsyasqt)
find_library(MARSYAS_LIBRARY marsyas)
find_library(MARSYASQT_LIBRARY marsyasqt)

if (EVE_STATIC)
  find_library(QSQLITE_LIBRARY qsqlite HINTS "${QT_PLUGINS_DIR}" PATH_SUFFIXES "sqldrivers")
ENDIF (EVE_STATIC)

if (MARSYAS_INCLUDE_DIR AND MARSYAS_LIBRARY AND MARSYASQT_LIBRARY)
	set(MARSYAS_FOUND TRUE)
endif (MARSYAS_INCLUDE_DIR AND MARSYAS_LIBRARY AND MARSYASQT_LIBRARY)

if (QSQLITE_LIBRARY)
  set(QSQLITE_FOUND TRUE)
endif (QSQLITE_LIBRARY)

if (MARSYAS_FOUND)
	if (NOT MARSYAS_FIND_QUIETLY)
		message (STATUS "Found MARSYAS: ${MARSYAS_LIBRARY}")
		message (STATUS "Found MARSYASQT: ${MARSYASQT_LIBRARY}")
	endif (NOT MARSYAS_FIND_QUIETLY)
else (MARSYAS_FOUND)
	if (MARSYAS_FIND_REQUIRED)
		message (FATAL_ERROR "Not found: MARSYAS")
	endif (MARSYAS_FIND_REQUIRED)
endif (MARSYAS_FOUND)

if (QSQLITE_FOUND)
    message (STATUS "Found QSQLITE: ${QSQLITE_LIBRARY}")
endif (QSQLITE_FOUND)
 
include_directories(${MARSYAS_INCLUDE_DIR} ${MARSYASQT_INCLUDE_DIR} ${PROJECT_BINARY_DIR})

cmake_minimum_required (VERSION 2.6)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

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
  util.cpp
  wizard.cpp
  userinterface.cpp
  )

set(APP_HEADERS
  mainwindow.h
  backend.h
  transport.h
  tagger.h
  experiment.h
  util.h
  wizard.h
  userinterface.h
  )

set(APP_FORMS
  mainwindow.ui
  transport.ui
  tagger.ui
  experiment.ui
  wizard.ui
  userinterface.ui
  )

set(APP_RESOURCES ../icons.qrc)

qt4_wrap_cpp(APP_HEADERS_MOC ${APP_HEADERS})
qt4_wrap_ui(APP_FORMS_HEADERS ${APP_FORMS})

QT4_ADD_RESOURCES(APP_RESOURCES_HEADERS ${APP_RESOURCES})

if (EVE_STATIC)
  set_target_properties(marsyas PROPERTIES IMPORTED_LOCATION ${MARSYAS_LIBRARY})
  set_target_properties(marsyasqt PROPERTIES IMPORTED_LOCATION ${MARSYASQT_LIBRARY})
  set_target_properties(qsqlite PROPERTIES IMPORTED_LOCATION ${QSQLITE_LIBRARY})
endif (EVE_STATIC)

add_executable (${APP_NAME} WIN32 MACOSX_BUNDLE ${APP_SOURCES} ${APP_HEADERS_MOC} ${APP_FORMS_HEADERS} ${APP_RESOURCES_HEADERS})

target_link_libraries (${APP_NAME} ${QT_LIBRARIES} ${EXTRA_LIBS} marsyas marsyasqt qsqlite)

install (TARGETS Eve DESTINATION bin)
install (FILES ${APP_HEADERS} "${PROJECT_BINARY_DIR}/config.h" DESTINATION include/Eve)

set (CPACK_PACKAGE_NAME "${APP_NAME}")
set (CPACK_BUNDLE_NAME "${APP_NAME}")
set (CPACK_BUNDLE_PLIST Info.plist)
set (CPACK_BUNDLE_ICON TagTool.png)
set (CPACK_PACKAGE_CONTACT "Pedro Silva <pasilva@inescporto.pt>")
set (CPACK_PACKAGE_VENDOR "INESC Porto")
set (CPACK_PACKAGE_VERSION_MAJOR ${EVE_VERSION_MAJOR})
set (CPACK_PACKAGE_VERSION_MINOR ${EVE_VERSION_MINOR})
set (CPACK_PACKAGE_VERSION_PATCH ${EVE_VERSION_PATCH})
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "Automated listening tests setup, evaluation and reporting")
set (CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/../COPYING")
set (CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/../README")
include(CPack)
