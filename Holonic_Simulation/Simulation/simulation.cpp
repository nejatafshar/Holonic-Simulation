#include "simulation.h"
#include "ui_Simulation.h"

#include <QMessageBox>

#include <QLabel>


Simulation::Simulation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Simulation)
{

    ui->setupUi(this);


}

Simulation::~Simulation()
{
    delete ui;

}
