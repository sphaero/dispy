#-------------------------------------------------
#
# Project created by QtCreator 2012-12-22T17:49:36
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = qtDispy
TEMPLATE = app


SOURCES += main.cpp\
    gldispywidget.cpp

HEADERS  += \
    gldispywidget.h


unix:LIBS += -lX11 -lXext
