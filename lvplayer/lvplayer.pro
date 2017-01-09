#-------------------------------------------------
#
# Project created by QtCreator 2017-01-06T13:33:46
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = lvplayer
TEMPLATE = app


SOURCES += main.cpp\
        lvplayer.cpp

HEADERS  += lvplayer.h

SDKPATH = $$PWD/sdk
INCLUDEPATH += $$SDKPATH/include
LIBS += -L$$SDKPATH/lib -lvlc
