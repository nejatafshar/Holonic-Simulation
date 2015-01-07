#include "simulation.h"
#include "ui_Simulation.h"

#include <QMessageBox>

#include <QLabel>


Simulation::Simulation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Simulation)
{

    ui->setupUi(this);

    root = NULL;


}

Simulation::~Simulation()
{
    delete ui;

}

void Simulation::initializeHolarchy(int levels, int holons)
{
    if(root)
        root->deleteLater();

    root = new Agent(NULL);

    initializeHolon(root, holons, 0, levels);
}

void Simulation::initializeHolon(Agent* parent, int holons, int level, int maxLevels)
{

    if(level == maxLevels)
        return;

    level++;

    for(int i=0;i<holons;i++)
    {
        Agent * agent = new Agent(parent);
        parent->addChild(agent);
        initializeHolon(agent, holons, level, maxLevels);
    }
}

void Simulation::on_startBut_clicked()
{
    int levels = ui->levels_lineEdit->text().toInt();
    int holons = ui->holons_lineEdit->text().toInt();

    initializeHolarchy(levels, holons);

    root->start();
}
