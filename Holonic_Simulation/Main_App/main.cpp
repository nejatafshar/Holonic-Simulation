#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    qApp->setOrganizationName("Home");
    qApp->setApplicationName("Holonic_Simulation");

    MainWindow w;
    w.showMaximized();

    return a.exec();
}
