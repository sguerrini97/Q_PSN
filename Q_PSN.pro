#-------------------------------------------------
#
# Project created by QtCreator 2014-11-14T16:50:31
#
#-------------------------------------------------

CONFIG += i386

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Q_PSN
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

RESOURCES += \
    res/res.qrc
