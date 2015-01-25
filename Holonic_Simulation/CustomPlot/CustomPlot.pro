#-------------------------------------------------
#
# Project created by QtCreator 2013-04-29T08:56:59
#
#-------------------------------------------------

QT       += core gui widgets printsupport

CONFIG += c++11

TARGET = CustomPlot
TEMPLATE = lib


DEFINES += QCUSTOMPLOT_COMPILE_LIBRARY

DEFINES += CUSTOMPLOT_LIBRARY

SOURCES += qcustomplot.cpp \
           customplot.cpp

HEADERS += qcustomplot.h \
           customplot.h \
           CustomPlot_global.h

RESOURCES += \
    CustomPlot_resources.qrc

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

win32:CONFIG(release, debug|release): DLLDESTDIR +=  $$PWD/../../Holonic_Simulation_Win
else:unix:CONFIG(release, debug|release): QMAKE_POST_LINK += $$quote(cp libCustomPlot.so.1  $$PWD/../../Holonic_Simulation_Linux)

#cause the dynamic linker to look in the same directory as my Qt application at runtime in Linux
unix:{
    # suppress the default RPATH if you wish
    QMAKE_LFLAGS_RPATH=
    # add your own with quoting gyrations to make sure $ORIGIN gets to the command line unexpanded
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
}

