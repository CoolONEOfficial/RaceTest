#-------------------------------------------------
#
# Project created by QtCreator 2016-05-04T15:48:18
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = raceTest
TEMPLATE = app

QMAKE_TARGET_COMPANY = CoolONE Inc.
QMAKE_TARGET_PRODUCT = RaceTest
QMAKE_TARGET_DESCRIPTION = RaceTest Game
QMAKE_TARGET_COPYRIGHT = Nickolay Trukhin

SOURCES += main.cpp\
        widget.cpp \
    car.cpp \
    wheel.cpp

HEADERS  += widget.h \
    car.h \
    wheel.h

FORMS    += widget.ui

RC_ICONS = icon.ico
