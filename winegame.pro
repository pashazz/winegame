# -------------------------------------------------
# Project created by QtCreator 2010-03-13T21:01:17
# -------------------------------------------------
TARGET = winegame
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    engine.cpp \
    discdetector.cpp \
    gamedialog.cpp
HEADERS += mainwindow.h \
    linux.h \
    engine.h \
    enginefunc.h \
    discdetector.h \
    gamedialog.h
TRANSLATIONS = l10n/wg_ru.ts
FORMS += mainwindow.ui \
    gamedialog.ui
RESOURCES += res.qrc
