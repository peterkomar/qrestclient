TEMPLATE = app
CONFIG += warn_on \
    thread \
    qt \
    #debug    #- uncoment for debug
    release #- uncoment for relese
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
    requestdetailsdlg.cpp \
    toolbar.cpp \
    leftpanel.cpp \
    rightpanel.cpp \
    bottompabel.cpp \
    menu.cpp \
    mainpanel.cpp \
    restclient.cpp \
    request.cpp \
    gist.cpp
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
    requestdetailsdlg.h \
    toolbar.h \
    leftpanel.h \
    rightpanel.h \
    bottompabel.h \
    menu.h \
    mainpanel.h \
    restclient.h \
    request.h \
    gist.h
RESOURCES += application.qrc
FORMS +=

ICON = qrestclient.icns
