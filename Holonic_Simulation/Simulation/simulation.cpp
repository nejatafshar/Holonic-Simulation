#include "simulation.h"
#include "ui_Simulation.h"

#include <QMessageBox>

#include <QLabel>

#include <QtMath>


Simulation::Simulation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Simulation)
{

    ui->setupUi(this);

    root = NULL;

    connect(ui->levels_lineEdit,&QLineEdit::textChanged, this, &Simulation::updateTotalHolons);
    connect(ui->holons_lineEdit,&QLineEdit::textChanged, this, &Simulation::updateTotalHolons);

    updateTotalHolons();


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

    connect(root, &Agent::simulationFinished, this, &Simulation::onSimulationFinished);

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

    elapsedTimer.start();

    root->start();
}

void Simulation::updateTotalHolons()
{
    int holons = ui->holons_lineEdit->text().toInt();
    int levels = ui->levels_lineEdit->text().toInt();
    ui->totalHolons_lineEdit->setText(QString("%1").arg(qPow(holons, levels)));
}

void Simulation::onSimulationFinished()
{
    qint64 elapsed = elapsedTimer.nsecsElapsed();

    ui->simulationTime_lineEdit->setText(QString::number(elapsed/1e9, 'f', 4));
}
