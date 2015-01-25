#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->mainToolBar->setHidden(true);
    ui->statusBar->setHidden(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}
