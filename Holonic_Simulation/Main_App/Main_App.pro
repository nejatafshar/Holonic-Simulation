#-------------------------------------------------
#
# Project created by QtCreator 2015-01-04T14:58:42
#
#-------------------------------------------------

QT       += widgets printsupport

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Holonic_Simulation
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc

win32: RC_ICONS = Holonic.ico


win32:CONFIG(release, debug|release): DLLDESTDIR +=  $$PWD/../../Holonic_Simulation_Win
else:linux:!android:CONFIG(release, debug|release): QMAKE_POST_LINK += $$quote(cp Holonic_Simulation $$PWD/../../Holonic_Simulation_Linux)

#cause the dynamic linker to look in the same directory as my Qt application at runtime in Linux
unix:!android:{
    # suppress the default RPATH if you wish
    QMAKE_LFLAGS_RPATH=
    # add your own with quoting gyrations to make sure $ORIGIN gets to the command line unexpanded
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
}


android:{
    OTHER_FILES = \
        android/AndroidManifest.xml

    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
}


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Simulation/release/ -lSimulation
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Simulation/debug/ -lSimulation
else:unix: LIBS += -L$$OUT_PWD/../Simulation/ -lSimulation

INCLUDEPATH += $$PWD/../Simulation
DEPENDPATH += $$PWD/../Simulation

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../CustomPlot/release/ -lCustomPlot
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../CustomPlot/debug/ -lCustomPlot
else:unix: LIBS += -L$$OUT_PWD/../CustomPlot/ -lCustomPlot

INCLUDEPATH += $$PWD/../CustomPlot
DEPENDPATH += $$PWD/../CustomPlot


