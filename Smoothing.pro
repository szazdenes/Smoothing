#-------------------------------------------------
#
# Project created by QtCreator 2014-07-23T15:33:57
#
#-------------------------------------------------

QT       += core gui widgets svg
CONFIG += qwt

TARGET = Smoothing
TEMPLATE = app
DESTDIR = ./bin
OBJECTS_DIR = ./obj
MOC_DIR = ./moc
RCC_DIR = ./rcc
UI_DIR = ./ui

LIBS += qwt/build/lib/libqwt.a

SOURCES += main.cpp\
        widget.cpp

HEADERS  += widget.h

FORMS    += widget.ui

