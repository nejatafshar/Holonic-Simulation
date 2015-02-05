#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setStyle("Fusion");

    qApp->setOrganizationName("University");
    qApp->setApplicationName("Holonic_Simulation");

    MainWindow w;
    w.showMaximized();

    return a.exec();
}
