QT += core
QT -= gui

CONFIG += c++11

TARGET = AviParser_Djh
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    avifileparser.cpp \
    avi.cpp

HEADERS += \
    avifileparser.h \
    byte_write.h \
    aiv.h
