#-------------------------------------------------
#
# Project created by QtCreator 2013-12-10T11:01:53
#
#-------------------------------------------------

QT       += core gui serialport script

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QLogger
TEMPLATE = app

CONFIG += console c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    loggerthread.cpp \
    logstorage.cpp \
    settings.cpp \
    texthighlighter.cpp \
    idhelper.cpp \
    customdialog.cpp \
    filterlist.cpp \
    taglist.cpp \
    typelist.cpp \
    logstream.cpp \
    comportstream.cpp \
    streamthread.cpp \
    threadmanager.cpp \
    filestream.cpp

HEADERS  += mainwindow.h \
    loggerthread.h \
    logstorage.h \
    settings.h \
    texthighlighter.h \
    idhelper.h \
    customdialog.h \
    debug.h \
    filterlist.h \
    taglist.h \
    typelist.h \
    abstractitem.h \
    logstream.h \
    comportstream.h \
    streamthread.h \
    threadmanager.h \
    filestream.h

FORMS    += mainwindow.ui
