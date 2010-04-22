# -------------------------------------------------
# Project created by QtCreator 2010-03-13T21:01:17
# -------------------------------------------------
TARGET = winegame
TEMPLATE = app
QT += network
SOURCES += main.cpp \
    mainwindow.cpp \
    discdetector.cpp \
    gamedialog.cpp \
    prefixdialog.cpp \
    isomaster.cpp \
    autopackage.cpp 
HEADERS += mainwindow.h \
    discdetector.h \
    gamedialog.h \
    prefixdialog.h \
    isomaster.h \
    autopackage.h 
FORMS += mainwindow.ui \
    gamedialog.ui \
    prefixdialog.ui 
INCLUDEPATH += libwinegame
RESOURCES += res.qrc
LIBS += -L/usr/lib \
    -lwinestuff
TRANSLATIONS += l10n/wg_ru.ts
VERSION = 0.0.1

# #################################################
# Install information                            #
# #################################################
tools.path = /usr/local/bin
tools.files = tools/*
pkgs.path = /usr/share/winegame
pkgs.files = packages/*
target.path = /usr/local/bin
desktop.path = /usr/share/applications
desktop.files = desktop/winegame.desktop
icon.path = /usr/share/pixmaps
icon.files = desktop/winegame.png
INSTALLS += target \
    pkgs \
    tools \
    desktop \
    icon
