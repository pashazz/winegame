# -------------------------------------------------
# Project created by QtCreator 2010-04-18T14:01:01
# -------------------------------------------------
QT += network \
    sql
TARGET = winestuff
TEMPLATE = lib
DEFINES += WINESTUFF_LIBRARY
SOURCES += engine.cpp \
    corelib.cpp \
    prefix.cpp
HEADERS += libwinegame_global.h \
    engine.h \
    corelib.h \
    prefix.h
isEmpty ($$PREFIX)
:PREFIX = /usr
target.path = $$PREFIX/lib
INSTALLS += target
VERSION = 0.0.1
FORMS += 
