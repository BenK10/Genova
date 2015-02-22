#-------------------------------------------------
#
# Project created by QtCreator 2015-01-29T19:34:46
#
#-------------------------------------------------

QT       += core gui script

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GENOVA
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    genova.cpp \
    genome.cpp

HEADERS  += mainwindow.h \
    genova.h \
    genome.h

FORMS    += mainwindow.ui

INCLUDEPATH += "C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Include"

RESOURCES += \
    gene.qrc

