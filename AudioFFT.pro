#-------------------------------------------------
#
# Project created by QtCreator 2016-07-09T15:10:01
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AudioFFT
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    datahub.cpp \
    audioinput.cpp \
    fftmodule.cpp \
    fft4g.c

HEADERS  += widget.h \
    datahub.h \
    audioinput.h \
    fftmodule.h

CONFIG += c++11
