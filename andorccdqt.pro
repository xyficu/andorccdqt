#-------------------------------------------------
#
# Project created by QtCreator 2015-01-17T21:49:00
#
#-------------------------------------------------

QT       += core gui
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = andorccdqt
TEMPLATE = app


SOURCES += main.cpp\
    image.cpp \
    andoruser.cpp \
    generic.cpp \
    mainwindow.cpp \
    options.cpp \
    andortcp.cpp

HEADERS  += \
    atmcdLXd.h \
    andoruser.h \
    ccdparams.h \
    mainwindow.h \
    options.h \
    andortcp.h

FORMS    += \
    mainwindow.ui \
    options.ui

DISTFILES +=

unix:!macx: LIBS += -L$$PWD/ -landor

INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/

unix:!macx: LIBS += -L$$PWD/ -lcfitsio

INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/

unix:!macx: PRE_TARGETDEPS += $$PWD/libcfitsio.a
