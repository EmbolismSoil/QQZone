#-------------------------------------------------
#
# Project created by QtCreator 2016-05-18T23:51:50
#
#-------------------------------------------------

QT       += core gui
QT += network
CONFIG += c++11
QT += qml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QQZone
TEMPLATE = app
target.path=/usr/local/bin
INSTALLS=target

SOURCES += main.cpp \
    qqhttp.cpp \
    qqzone.cpp \
    myhttp.cpp \
    tulingrobot.cpp \
    abstractrobot.cpp

HEADERS  += \
    qqhttp.h \
    qqzone.h \
    myhttp.h \
    tulingrobot.h \
    abstractrobot.h

FORMS    += mainwindow.ui

DISTFILES += \
    tx.js
