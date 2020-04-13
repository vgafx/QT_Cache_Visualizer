#-------------------------------------------------
#
# Project created by QtCreator 2019-06-16T13:47:55
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CacheVisualizer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17


SOURCES += \
        backgroundworker.cpp \
        block.cpp \
        cache.cpp \
        cacheline.cpp \
        executionsimulation.cpp \
        filehandler.cpp \
        main.cpp \
        cachevisualizer.cpp \
        runtimeflags.cpp \
        scopedtimer.cpp \
        settings.cpp \
        simulationstatistics.cpp \
        statuscontroller.cpp \
        view.cpp

HEADERS += \
        backgroundworker.h \
        block.h \
        cache.h \
        cacheline.h \
        cachevisualizer.h \
        datadefinitions.h \
        executionsimulation.h \
        filehandler.h \
        main.h \
        runtimeflags.h \
        scopedtimer.h \
        settings.h \
        simulationstatistics.h \
        statuscontroller.h \
        view.h

FORMS += \
        cachevisualizer.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
