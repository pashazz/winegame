# -------------------------------------------------
# Project created by QtCreator 2010-03-13T21:01:17
# -------------------------------------------------
TARGET = winegame
TEMPLATE = app
QT += network
SOURCES += main.cpp \
    mainwindow.cpp \
    engine.cpp \
    discdetector.cpp \
    gamedialog.cpp \
    prefix.cpp \
    prefixdialog.cpp
HEADERS += mainwindow.h \
    linux.h \
    engine.h \
    enginefunc.h \
    discdetector.h \
    gamedialog.h \
    prefix.h \
    prefixdialog.h
TRANSLATIONS = l10n/wg_ru.ts
FORMS += mainwindow.ui \
    gamedialog.ui \
    prefixdialog.ui
RESOURCES += res.qrc
