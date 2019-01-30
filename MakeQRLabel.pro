#-------------------------------------------------
#
# Project created by QtCreator 2016-04-29T09:46:25
#
#-------------------------------------------------

QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MakeQRLabel
TEMPLATE = app

DEFINES += HAVE_CONFIG_H
INCLUDEPATH += libqrencode

SOURCES += main.cpp\
        mainwindow.cpp \
    libqrencode/bitstream.c \
    libqrencode/mask.c \
    libqrencode/mmask.c \
    libqrencode/mqrspec.c \
    libqrencode/qrenc.c \
    libqrencode/qrencode.c \
    libqrencode/qrinput.c \
    libqrencode/qrspec.c \
    libqrencode/rsecc.c \
    libqrencode/split.c

HEADERS  += mainwindow.h \
    libqrencode/bitstream.h \
    libqrencode/mask.h \
    libqrencode/mmask.h \
    libqrencode/mqrspec.h \
    libqrencode/qrencode.h \
    libqrencode/qrencode_inner.h \
    libqrencode/qrinput.h \
    libqrencode/qrspec.h \
    libqrencode/rsecc.h \
    libqrencode/split.h \
    libqrencode/config.h

FORMS    += mainwindow.ui
