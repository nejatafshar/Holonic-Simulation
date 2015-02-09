#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setStyle("Fusion");

    qApp->setOrganizationName("IUST");
    qApp->setApplicationName("Holonic_Simulation");

    MainWindow w;
    w.showFullScreen();

    return a.exec();
}
