# -------------------------------------------------
# Project created by QtCreator 2010-04-18T14:01:01
# -------------------------------------------------
QT += network
TARGET = winestuff
TEMPLATE = lib
DEFINES += WINESTUFF_LIBRARY
SOURCES += engine.cpp \
    corelib.cpp
HEADERS += libwinegame_global.h \
    engine.h \
    enginefunc.h \
    linux.h \
    corelib.h
target.path = /usr/lib/
INSTALLS += target
VERSION = 0.0.1
