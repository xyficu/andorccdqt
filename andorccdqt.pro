#-------------------------------------------------
#
# Project created by QtCreator 2015-01-17T21:49:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = andorccdqt
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    image.cpp \
    andoruser.cpp

HEADERS  += widget.h \
    atmcdLXd.h \
    andoruser.h

FORMS    += widget.ui

DISTFILES +=

unix:!macx: LIBS += -L$$PWD/ -landor

INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/
