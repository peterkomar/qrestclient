TEMPLATE = app
CONFIG += warn_on \
    thread \
    qt \
    #debug
    release #- uncoment for relese
TARGET = ../bin/qrestclient
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
RCC_DIR = build
SOURCES += main.cpp \
    editdlgimpl.cpp \
    requesthistory.cpp \
    restclientmainwindow.cpp \
    paramslist.cpp \
    resthistorywidget.cpp
QT += network \
    sql \
    widgets
HEADERS += \
    editdlgimpl.h \
    requesthistory.h \
    restclientmainwindow.h \
    paramslist.h \
    resthistorywidget.h
RESOURCES += application.qrc
FORMS += \
    dialog.ui
