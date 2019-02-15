#-------------------------------------------------
#
# Project created by QtCreator 2019-01-04T02:07:12
#
#-------------------------------------------------

QT       += core gui network xml multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sub_ping_3
TEMPLATE = app

LIBS += -L"$$_PRO_FILE_PWD_/pugixml/"
#LIBS += -L"$$_PRO_FILE_PWD_/pugixml/" -lpsapi
#LIBS += -L"$$_PRO_FILE_PWD_/pugixml/" -lmylib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        sub_ping_3.cpp \
    authdialog.cpp \
    chumlistitem.cpp \
    clickablelist.cpp

HEADERS += \
        sub_ping_3.h \
    authdialog.h \
    chumlistitem.h \
    clickablelist.h

FORMS += \
        sub_ping_3.ui \
    authdialog.ui

#newcomment
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
