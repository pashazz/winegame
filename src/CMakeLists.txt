cmake_minimum_required( VERSION 2.6 )
project (winegame)

if (${CMAKE_SYSTEM_NAME} MATCHES Linux)
  message (STATUS "Building WineGame on Linux")
else (${CMAKE_SYSTEM_NAME} MATCHES Linux)
  message (FATAL ERROR "Only Linux is supported by WineGame")
endif (${CMAKE_SYSTEM_NAME} MATCHES Linux)

find_package( Qt4 REQUIRED )
            IF(QT_QTSQL_FOUND)
                SET(QT_USE_QTSQL 1)
            ELSE(QT_QTSQL_FOUND)
                MESSAGE(FATAL ERROR "Qt SQL libraries missing.")
            ENDIF(QT_QTSQL_FOUND)

            IF(QT_QTGUI_FOUND)
                SET(QT_USE_QTGUI 1)
            ELSE(QT_QTGUI_FOUND)
                MESSAGE(FATAL ERROR "Qt GUI libraries missing.")
            ENDIF(QT_QTGUI_FOUND)

            IF(QT_QTNETWORK_FOUND)
                SET(QT_USE_QTNETWORK 1)
            ELSE(QT_QTNETWORK_FOUND)
                MESSAGE(FATAL ERROR "Qt NetWork libraries missing.")
            ENDIF(QT_QTNETWORK_FOUND)

            IF(QT_QTCORE_FOUND)
                SET(QT_USE_QTCORE 1)
            ELSE(QT_QTCORE_FOUND)
                MESSAGE(FATAL ERROR "Qt Core libraries missing.")
            ENDIF(QT_QTCORE_FOUND)
#THANKS TO Q4WINE PROJ.

include( ${QT_USE_FILE} )

find_program( FUSEISO_BIN NAMES fuseiso )
#MESSAGE(STATUS "Searching for fuseiso binary --\n")
IF (${FUSEISO_BIN} MATCHES FUSEISO_BIN-NOTFOUND)
    MESSAGE(STATUS "    fuseiso : NOT FOUND!\n")
    MESSAGE(STATUS "    WARNING: To have ability to mount and umount ISO9660 images")
    MESSAGE(STATUS "    WARNING: without root privileges you may wish to install")
    MESSAGE(STATUS "    WARNING: fuseiso package ( http://fuseiso.sourceforge.net/ )\n")
ENDIF (${FUSEISO_BIN} MATCHES FUSEISO_BIN-NOTFOUND)

#Ищем winestuff
find_package(WineStuff REQUIRED)
#message(STATUS "Searching for WineStuff")
find_package(WineStuff REQUIRED)
#message(STATUS "WineStuff Include Dir: ${WINESTUFF_INCLUDE_DIR}")
#message(STATUS "WineStuff Library: ${WINESTUFF_LIBRARIES}")
set (CMAKE_BUILD_TYPE Debug)
include_directories(${WINESTUFF_INCLUDE_DIR})
#message (STATUS "Files will be compiled in: ${CMAKE_CURRENT_BINARY_DIR}")
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set (RESDIR ${CMAKE_SOURCE_DIR}/resources)

# Sources, Headers
set (SOURCES about.cpp  diskdialog.cpp  gamedialog.cpp  main.cpp  mainwindow.cpp  prefixdialog.cpp  settingsdialog.cpp  shortcutdialog.cpp  winegameui.cpp  wineversionsdialog.cpp treemodel.cpp package.cpp plugindialog.cpp feedbackdialog.cpp ejectdialog.cpp nextdisc.cpp appsettings.cpp winetricksmodel.cpp trixdialog.cpp)
set (QOBJ_HEADERS about.h  diskdialog.h  gamedialog.h  mainwindow.h  prefixdialog.h  settingsdialog.h  shortcutdialog.h  wineversionsdialog.h treemodel.h plugindialog.h feedbackdialog.h ejectdialog.h nextdisc.h appsettings.h winetricksmodel.h trixdialog.h)
set (HEADERS package.h winegameui.h)

#Генерируем формы
set (UIS about.ui  diskdialog.ui  gamedialog.ui  mainwindow.ui  prefixdialog.ui  settingsdialog.ui  shortcutdialog.ui  wineversionsdialog.ui plugindialog.ui feedbackdialog.ui ejectdialog.ui nextdisc.ui trixdialog.ui)
qt4_wrap_ui (UI_HEADERS ${UIS})

#Генерируем ресурсы
set (RES ${RESDIR}/res.qrc)
qt4_add_resources (QRC_SOURCES ${RES})

#Генерируем файлы MOC
qt4_wrap_cpp (MOC_SOURCES ${QOBJ_HEADERS})


#Translations
set (TSDIR ${CMAKE_CURRENT_SOURCE_DIR}/l10n)
set (LANGUAGES wg_ru)

foreach (LANGUAGE ${LANGUAGES})
set (TS ${TSDIR}/${LANGUAGE}.ts)
set (QM ${TSDIR}/${LANGUAGE}.qm)
set (TRANSLATIONS ${TRANSLATIONS} ${TS})
set (TRANSLATIONS_BINARY ${TRANSLATIONS_BINARY} ${QM})
add_custom_command (
OUTPUT ${QM}
COMMAND ${QT_LRELEASE_EXECUTABLE} ${TS}
WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/l10n
MAIN_DEPENDENCY ${TS})
endforeach()
add_custom_target (
translations
COMMAND ${QT_LUPDATE_EXECUTABLE} ${MOC_SOURCES} ${SOURCES} ${UIS} -ts ${TRANSLATIONS} -I${CMAKE_CURRENT_BINARY_DIR}
WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_custom_command (
TARGET translations
COMMAND ${QT_LRELEASE_EXECUTABLE} ${TRANSLATIONS}
WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable (winegame ${SOURCES} ${HEADERS} ${MOC_SOURCES} ${UI_HEADERS} ${QRC_SOURCES} ${TRANS})

#Линкуем
target_link_libraries( winegame ${QT_LIBRARIES} ${WINESTUFF_LIBRARIES})

#ставим
install (TARGETS winegame DESTINATION bin)
install (FILES ${TRANSLATIONS_BINARY} DESTINATION share/winegame/translations)
install (FILES ${CMAKE_SOURCE_DIR}/desktop/winegame.png DESTINATION share/pixmaps)
install (FILES ${CMAKE_SOURCE_DIR}/desktop/winegame.desktop DESTINATION share/applications)
