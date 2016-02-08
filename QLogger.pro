#-------------------------------------------------
#
# Project created by QtCreator 2013-12-10T11:01:53
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QLogger
TEMPLATE = app

CONFIG += console

SOURCES += main.cpp\
        mainwindow.cpp \
    loggerthread.cpp \
    logstorage.cpp \
    settings.cpp \
    adblogcatthread.cpp \
    texthighlighter.cpp \
    tzlogthread.cpp \
    idhelper.cpp \
    customdialog.cpp \
    filterlist.cpp \
    taglist.cpp \
    typelist.cpp \
    filethread.cpp \
    adblogcat.cpp \
    kernellogthread.cpp

HEADERS  += mainwindow.h \
    loggerthread.h \
    logstorage.h \
    settings.h \
    adblogcatthread.h \
    texthighlighter.h \
    tzlogthread.h \
    idhelper.h \
    customdialog.h \
    debug.h \
    filterlist.h \
    taglist.h \
    typelist.h \
    abstractitem.h \
    filethread.h \
    adblogcat.h \
    kernellogthread.h

FORMS    += mainwindow.ui
