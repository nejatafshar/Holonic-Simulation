#include "simulation.h"
#include "ui_Simulation.h"

#include <QMessageBox>

#include <QLabel>

#include <QtMath>

#include <QSettings>


Simulation::Simulation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Simulation)
{

    ui->setupUi(this);

    QSettings settings;
    ui->levels_lineEdit->setText(settings.value("SimulationSettings/levels","3").toString());
    ui->holons_lineEdit->setText(settings.value("SimulationSettings/holons","5").toString());
    ui->maxCycles_lineEdit->setText(settings.value("SimulationSettings/maxCycles","10").toString());
    ui->desiredVariance_lineEdit->setText(settings.value("SimulationSettings/desiredVariance","1").toString());
    ui->agentNeeds_lineEdit->setText(settings.value("SimulationSettings/agentNeeds","10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10").toString());
    ui->resourceStandardDeviation_lineEdit->setText(settings.value("SimulationSettings/standardDeviation","2").toString());
    ui->priorities_lineEdit->setText(settings.value("SimulationSettings/priorities","50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50").toString());
    ui->prioritiesStandardDeviation_lineEdit->setText(settings.value("SimulationSettings/prioritiesStandardDeviation","2").toString());


    root = NULL;

    connect(ui->levels_lineEdit,&QLineEdit::textChanged, this, &Simulation::updateTotalHolons);
    connect(ui->holons_lineEdit,&QLineEdit::textChanged, this, &Simulation::updateTotalHolons);

    updateTotalHolons();


}

Simulation::~Simulation()
{
    QSettings settings;
    settings.setValue("SimulationSettings/levels",ui->levels_lineEdit->text());
    settings.setValue("SimulationSettings/holons",ui->holons_lineEdit->text());
    settings.setValue("SimulationSettings/maxCycles",ui->maxCycles_lineEdit->text());
    settings.setValue("SimulationSettings/desiredVariance",ui->desiredVariance_lineEdit->text());
    settings.setValue("SimulationSettings/agentNeeds",ui->agentNeeds_lineEdit->text());
    settings.setValue("SimulationSettings/standardDeviation",ui->resourceStandardDeviation_lineEdit->text());
    settings.setValue("SimulationSettings/priorities",ui->priorities_lineEdit->text());
    settings.setValue("SimulationSettings/prioritiesStandardDeviation",ui->prioritiesStandardDeviation_lineEdit->text());

    delete ui;
}

void Simulation::initializeHolarchy(int levels, int holons)
{
    if(root)
        root->deleteLater();

    root = new Agent(NULL);
    root->setMaxFutileCycles(ui->maxCycles_lineEdit->text().toInt());
    root->setDesiredVariance(ui->desiredVariance_lineEdit->text().toDouble());

    connect(root, &Agent::simulationFinished, this, &Simulation::onSimulationFinished);

    //Initialize variables
    meanResources.clear();
    QStringList list1 = ui->agentNeeds_lineEdit->text().split(",");
    foreach(QString item, list1)
    {
        meanResources.append(item.toUShort());
    }
    resourceStandardDeviation = ui->resourceStandardDeviation_lineEdit->text().toDouble();

    meanPriorities.clear();
    QStringList list2 = ui->priorities_lineEdit->text().split(",");
    foreach(QString item, list2)
    {
        meanPriorities.append(item.toUShort());
    }
    priorityStandardDeviation = ui->prioritiesStandardDeviation_lineEdit->text().toDouble();

    //Make Holarchy
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

        QVector<ushort> resources;
        QVector<ushort> priorities;
        resources.resize(ResourceElements);

        for(int i=0; i<ResourceElements; i++)
        {
            double val;
            statistics.gaussianRandomGererator(meanResources[i], resourceStandardDeviation, 1, &val);
            resources[i] = (ushort)val;
            statistics.gaussianRandomGererator(meanPriorities[i], priorityStandardDeviation, 1, &val);
            priorities[i] = (ushort)val;
        }

        agent->setResources(resources);
        agent->setPriorities(priorities);

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
