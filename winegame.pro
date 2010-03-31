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
    prefixdialog.cpp \
    isomaster.cpp
HEADERS += mainwindow.h \
    linux.h \
    engine.h \
    enginefunc.h \
    discdetector.h \
    gamedialog.h \
    prefix.h \
    prefixdialog.h \
    isomaster.h
TRANSLATIONS = l10n/wg_ru.ts
FORMS += mainwindow.ui \
    gamedialog.ui \
    prefixdialog.ui
RESOURCES += res.qrc
tools.path = /usr/bin
tools.files = tools/*
pkgs.path = /usr/share/winegame
pkgs.files = packages/*
prog.path = /usr/bin
prog.files = winegame
desktop.path = /usr/share/applications
desktop.files = desktop/winegame.desktop
icon.path = /usr/share/pixmaps
icon.files = desktop/winegame.png
INSTALLS += prog \
    pkgs \
    tools \
    desktop \
    icon
