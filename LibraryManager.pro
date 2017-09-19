#-------------------------------------------------
#
# Project created by QtCreator 2017-02-21T11:07:43
#
#-------------------------------------------------

QT       += core gui sql printsupport  quickwidgets
#webenginewidgets
CONFIG   += c++11
TARGET   = LibraryManager
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    tabwidgetclients.cpp \
    tabwidgetworkers.cpp \
    tabwidgetdocuments.cpp \
    topnav.cpp \
    login.cpp \
    archiveddoc.cpp \
    activedoc.cpp \
    simplecrypt.cpp \
    tabwidgetsettings.cpp \
    dialog.cpp \
    mdialog.cpp \
    toast.cpp \
    dbh.cpp

HEADERS  += mainwindow.h \
    tabwidgetclients.h \
    tabwidgetworkers.h \
    tabwidgetdocuments.h \
    topnav.h \
    login.h \
    archiveddoc.h \
    activedoc.h \
    simplecrypt.h \
    tabwidgetsettings.h \
    dialog.h \
    mdialog.h \
    toast.h \
    dbh.h \
    classes.h

FORMS    += mainwindow.ui \
    tabwidgetclients.ui \
    tabwidgetworkers.ui \
    tabwidgetdocuments.ui \
    topnav.ui \
    login.ui \
    archiveddoc.ui \
    activedoc.ui \
    tabwidgetsettings.ui \
    dialog.ui \
    mdialog.ui \
    toast.ui

RESOURCES += \
    Icons.qrc \
    languages.qrc \
    sounds.qrc \
    styles.qrc

TRANSLATIONS = LibraryManager_ar.ts

DISTFILES += \
    logFile.ini


