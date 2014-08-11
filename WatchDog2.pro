#-------------------------------------------------
#
# Project created by QtCreator 2014-07-02T06:58:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WatchDog2
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    template.cpp \
    newinteractivityform.cpp \
    js.cpp \
    configHandler.cpp \
    css.cpp \
    locacc.cpp \
    appconfig.cpp

HEADERS  += mainwindow.h \
    templates.h \
    newinteractivityform.h \
    js.h \
    Monitor.h \
    configHandler.h \
    css.h \
    locacc.h \
    messages.h \
    appconfig.h

FORMS    += \
    newinteractivityform.ui \
    mainwindow.ui

OTHER_FILES +=

RESOURCES += \
    resources.qrc
