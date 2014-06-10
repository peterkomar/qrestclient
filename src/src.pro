TEMPLATE = app
CONFIG += warn_on \
    thread \
    qt \
<<<<<<< HEAD
    debug    #- uncoment for debug
    #release #- uncoment for relese
=======
    #debug
    release #- uncoment for relese
>>>>>>> 7462c7fa66007418bdca926c03f2d768eb16a63d
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
    resthistorywidget.cpp \
    qjsonview.cpp \
    responsewidget.cpp
QT += network \
    sql \
    widgets
HEADERS += \
    editdlgimpl.h \
    requesthistory.h \
    restclientmainwindow.h \
    paramslist.h \
    resthistorywidget.h \
    qjsonview.h \
    responsewidget.h
RESOURCES += application.qrc
FORMS += \
    dialog.ui
