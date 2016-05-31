#-------------------------------------------------
#
# Project created by QtCreator 2016-05-04T15:48:18
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = raceTrack
TEMPLATE = app

QMAKE_TARGET_COMPANY = CoolONE Inc.
QMAKE_TARGET_PRODUCT = RaceTrack
QMAKE_TARGET_DESCRIPTION = RaceTrack Game
QMAKE_TARGET_COPYRIGHT = Nickolay Trukhin

SOURCES += main.cpp\
        widget.cpp \
    car.cpp \
    wheel.cpp \
    cmask.cpp \
    animation.cpp \
    animationcoords.cpp \
    button.cpp

HEADERS  += widget.h \
    car.h \
    wheel.h \
    cmask.h \
    animation.h \
    animationcoords.h \
    button.h

FORMS    += widget.ui

RC_ICONS = icon.ico

RESOURCES += \
    images.qrc \
    fonts.qrc

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
