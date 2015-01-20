#-------------------------------------------------
#
# Project created by QtCreator 2014-07-02T06:58:52
#
#-------------------------------------------------

QT      +=   core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WatchDog2
TEMPLATE = app


SOURCES +=  main.cpp\
    mainwindow.cpp \
    template.cpp \
    newinteractivityform.cpp \
    js.cpp \
    configHandler.cpp \
    css.cpp \
    locacc.cpp \
    appconfig.cpp \
    mediaimages.cpp \
    components.cpp \
    exporthelp.cpp

HEADERS  += mainwindow.h \
    templates.h \
    newinteractivityform.h \
    js.h \
    configHandler.h \
    css.h \
    locacc.h \
    messages.h \
    appconfig.h \
    mediaimages.h \
    components.h \
    exporthelp.h

FORMS    += \
    newinteractivityform.ui \
    mainwindow.ui \
    exporthelp.ui

win32:RC_ICONS += tray_icon.ico

RC_FILE = watchdog2.rc

RESOURCES += \
    resources.qrc


