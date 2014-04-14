TEMPLATE = app
CONFIG += warn_on \
    thread \
    qt \
    release
TARGET = ../bin/qrestclient
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
RCC_DIR = build
SOURCES += main.cpp \
    restdialogimpl.cpp \
    editdlgimpl.cpp \
    requesthistory.cpp
QT += network \
    sql \
    widgets
HEADERS += \
    restdialogimpl.h \
    editdlgimpl.h \
    requesthistory.h
RESOURCES += application.qrc
FORMS += \
    form.ui \
    dialog.ui
