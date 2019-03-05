#-------------------------------------------------
#
# Project created by QtCreator 2019-02-15T16:56:18
#
#-------------------------------------------------

QT       += core gui xml multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = chumstreamer_desktop
TEMPLATE = app
INCLUDEPATH += .
include(../chumstreamer_core/chumstreamer_core.pri)

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# make suppress debug output when we compile in release mode
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        chumstreamer_desktop.cpp \
    clickablelist.cpp \
    authdialog.cpp

HEADERS += \
        chumstreamer_desktop.h \
    #musicfolderinfo.h \
    clickablelist.h \
    chumlistitem.h \
    authdialog.h

FORMS += \
        chumstreamer_desktop.ui \
    authdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
