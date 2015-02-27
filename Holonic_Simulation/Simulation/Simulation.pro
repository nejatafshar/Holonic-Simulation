#-------------------------------------------------
#
# Project created by QtCreator 2013-05-18T11:45:18
#
#-------------------------------------------------

QT       += widgets printsupport

CONFIG += c++11

TARGET = Simulation
TEMPLATE = lib

DEFINES += SIMULATION_LIBRARY

SOURCES += \
    simulation.cpp \
    agent.cpp \
    statistics.cpp

HEADERS += \
    Simulation_global.h \
    simulation.h \
    agent.h \
    statistics.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

FORMS += \
    Simulation.ui


win32:CONFIG(release, debug|release): DLLDESTDIR +=  $$PWD/../../Holonic_Simulation_Win
else:linux:!android:CONFIG(release, debug|release): QMAKE_POST_LINK += $$quote(cp libSimulation.so.1  $$PWD/../../Holonic_Simulation_Linux)


#cause the dynamic linker to look in the same directory as my Qt application at runtime in Linux
unix:{
    # suppress the default RPATH if you wish
    QMAKE_LFLAGS_RPATH=
    # add your own with quoting gyrations to make sure $ORIGIN gets to the command line unexpanded
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../CustomPlot/release/ -lCustomPlot
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../CustomPlot/debug/ -lCustomPlot
else:unix: LIBS += -L$$OUT_PWD/../CustomPlot/ -lCustomPlot

INCLUDEPATH += $$PWD/../CustomPlot
DEPENDPATH += $$PWD/../CustomPlot
