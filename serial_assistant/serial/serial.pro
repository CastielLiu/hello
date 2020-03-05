#-------------------------------------------------
#
# Project created by QtCreator 2019-04-22T14:10:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -std=c++11

TARGET = serial-assistant
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.

DEFINES += QT_DEPRECATED_WARNINGS
INCLUDEPATH += /usr/include/boost
LIBS +=/usr/lib/x86_64-linux-gnu/libboost_thread.a
LIBS +=/usr/lib/x86_64-linux-gnu/libboost_system.a
DEFINES +=BOOST_USE_LIB

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    handle.cpp \
    serial/src/list_ports_linux.cc \
    serial/src/list_ports_osx.cc \
    serial/src/list_ports_win.cc \
    serial/src/serial.cc \
    serial/src/unix.cc \
    serial/src/win.cc

HEADERS += \
        mainwindow.h \
    handle.h \
    serial/serial.h \
    serial/v8stdint.h \
    serial/impl/unix.h \
    serial/impl/win.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    resources/res.qrc

RC_FILE += resources/logo.rc

DISTFILES +=

