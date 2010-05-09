# -------------------------------------------------
# Project created by QtCreator 2010-03-13T21:01:17
# -------------------------------------------------
TARGET = winegame
TEMPLATE = app
QT += network \
    sql
SOURCES += main.cpp \
    mainwindow.cpp \
    discdetector.cpp \
    gamedialog.cpp \
    prefixdialog.cpp \
    isomaster.cpp \
    diskdialog.cpp \
    settingsdialog.cpp \
    about.cpp \
    shortcutdialog.cpp
HEADERS += mainwindow.h \
    discdetector.h \
    gamedialog.h \
    prefixdialog.h \
    isomaster.h \
    diskdialog.h \
    settingsdialog.h \
    about.h \
    shortcutdialog.h
FORMS += mainwindow.ui \
    gamedialog.ui \
    prefixdialog.ui \
    diskdialog.ui \
    settingsdialog.ui \
    about.ui \
    shortcutdialog.ui
INCLUDEPATH += libwinegame
RESOURCES += res.qrc
LIBS += -L \
    $$PREFIX/lib \
    -lwinestuff
TRANSLATIONS += l10n/wg_ru.ts
VERSION = 0.0.1

# #################################################
# Install information                            #
# ################################################
# PREFIX=$(DESTDIR)
message ("WineGame version $$VERSION")
isEmpty (PREFIX) { 
    PREFIX = /usr
    message ("PREFIX is automatically set to /usr")
}
message ("Winegame packages is always in /usr/share/winegame")
message ("Checking for fuseISO")
system ("which fuseiso"):FUSEISO_BIN = FALSE # fuseiso check
pkgs.path = /usr/share/winegame
pkgs.files = packages/*
target.path = $$PREFIX/bin
desktop.path = $$PREFIX/share/applications
desktop.files = desktop/winegame.desktop
icon.path = $$PREFIX/share/pixmaps
icon.files = desktop/winegame.png
INSTALLS += target \
    pkgs \
    desktop \
    icon
