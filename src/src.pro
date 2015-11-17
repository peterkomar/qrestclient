TEMPLATE = app
CONFIG += warn_on \
    thread \
    qt \
    debug    #- uncoment for debug
    #release #- uncoment for relese
TARGET = ../bin/qrestclient
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
RCC_DIR = build
SOURCES += main.cpp \
    requesthistory.cpp \
    restclientmainwindow.cpp \
    paramslist.cpp \
    resthistorywidget.cpp \
    qjsonview.cpp \
    responsewidget.cpp \
    qcsvview.cpp \
    paramdlg.cpp \
    requestdetailsdlg.cpp
QT += network \
    sql \
    widgets
HEADERS += \
    requesthistory.h \
    restclientmainwindow.h \
    paramslist.h \
    resthistorywidget.h \
    qjsonview.h \
    responsewidget.h \
    qcsvview.h \
    paramdlg.h \
    requestdetailsdlg.h
RESOURCES += application.qrc
FORMS +=

ICON = qrestclient.icns
