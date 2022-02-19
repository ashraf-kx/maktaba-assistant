#-------------------------------------------------
#
# Project created by QtCreator 2017-02-21T11:07:43
#
#-------------------------------------------------

QT       += core gui sql printsupport

TARGET = LibraryAssistant
TEMPLATE = app

SOURCES += main.cpp\
    activedocuments.cpp \
    archiveddocuments.cpp \
        mainwindow.cpp \
    db.cpp \
    tabwidgetclients.cpp \
    tabwidgetworkers.cpp \
    tabwidgetdocuments.cpp \
    topnav.cpp \
    login.cpp \
    simplecrypt.cpp \
    tabwidgetsettings.cpp \
    dialog.cpp \
    toast.cpp

HEADERS  += mainwindow.h \
    activedocuments.h \
    archiveddocuments.h \
    db.h \
    tabwidgetclients.h \
    tabwidgetworkers.h \
    tabwidgetdocuments.h \
    topnav.h \
    login.h \
    simplecrypt.h \
    tabwidgetsettings.h \
    dialog.h \
    toast.h

FORMS    += mainwindow.ui \
    activedocuments.ui \
    archiveddocuments.ui \
    tabwidgetclients.ui \
    tabwidgetworkers.ui \
    tabwidgetdocuments.ui \
    topnav.ui \
    login.ui \
    tabwidgetsettings.ui \
    dialog.ui \
    toast.ui

RESOURCES += \
    Icons.qrc \
    lang.qrc

TRANSLATIONS = LibraryAssistant_ar.ts


