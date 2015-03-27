#-------------------------------------------------
#
# Project created by QtCreator 2015-03-23T17:35:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Opencv_example
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    surf.cpp \
    ellipse.cpp

HEADERS  += mainwindow.h \
    surf.h \
    ellipse.h

FORMS    += mainwindow.ui

LIBS += `pkg-config opencv --libs`
