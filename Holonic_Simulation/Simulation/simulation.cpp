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

    peakLoads.insert(0, ResourceElements, 0);

    QSettings settings;
    ui->levels_lineEdit->setText(settings.value("SimulationSettings/levels","3").toString());
    ui->holons_lineEdit->setText(settings.value("SimulationSettings/holons","5").toString());
    ui->maxCycles_lineEdit->setText(settings.value("SimulationSettings/maxCycles","10").toString());
    ui->desiredVariance_lineEdit->setText(settings.value("SimulationSettings/desiredVariance","1").toString());
    ui->agentNeeds_lineEdit->setText(settings.value("SimulationSettings/agentNeeds","10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10").toString());
    ui->resourceStandardDeviation_lineEdit->setText(settings.value("SimulationSettings/standardDeviation","2").toString());
    ui->priorities_lineEdit->setText(settings.value("SimulationSettings/priorities","50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50").toString());
    ui->prioritiesStandardDeviation_lineEdit->setText(settings.value("SimulationSettings/prioritiesStandardDeviation","2").toString());

    initializePlot();

    peakLoadPlotTimer.setInterval(30);
    connect(&peakLoadPlotTimer, &QTimer::timeout, this, &Simulation::updatePeakLoadPlot );


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


    settings.setValue("peakLoadPlot/penColor",ui->peakLoadPlot->graph(0)->pen().color());
    settings.setValue("peakLoadPlot/brushColor",ui->peakLoadPlot->graph(0)->brush().color());
    settings.setValue("peakLoadPlot/lineStyle",ui->peakLoadPlot->graph(0)->lineStyle());
    settings.setValue("peakLoadPlot/pointStyle",ui->peakLoadPlot->graph(0)->scatterStyle().shape());

    delete ui;
}

void Simulation::initializePlot()
{
    CustomPlot * plot= qobject_cast<CustomPlot *>(ui->peakLoadPlot);
    if (plot)
    {

        plot->xAxis->setLabel("");
        plot->legend->setVisible(false);
        plot->xAxis->setRange(0,25);
        plot->yAxis->setRange(0,100);
        plot->xAxis->setTicks(true);
        plot->xAxis2->setTicks(false);


//        plot->setBackground(QColor(10,10,10));
//        plot->xAxis->setTickLabelColor(QColor(225,225,225));
//        plot->yAxis->setTickLabelColor(QColor(225,225,225));
//        plot->xAxis->setLabelColor(QColor(225,225,225));
//        plot->yAxis->setLabelColor(QColor(225,225,225));

        plot->yAxis->setLabel("KW/h");

        plot->addGraph("",0,0,0);

        plot->graph()->setLineStyle(QCPGraph::lsLine);

        plot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));

        plot->yAxis->setSubTickCount(0);

        plot->autoRescale=true;
        plot->rescaleYWithMargin=true;

        QSettings settings;
        plot->setPenColor(0, settings.value("peakLoadPlot/penColor",QColor(rand()%245+10, rand()%245+10, rand()%245+10)).value<QColor>() );
        plot->setBrushColor(0, settings.value("peakLoadPlot/brushColor",QColor(rand()%245+10, rand()%245+10, rand()%245+10)).value<QColor>() );
        plot->graph()->setLineStyle((QCPGraph::LineStyle)settings.value("peakLoadPlot/lineStyle",QCPGraph::lsLine).toInt());
        plot->graph()->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape) settings.value("peakLoadPlot/pointStyle",QCPScatterStyle::ssDisc).toInt() ));

    }
}

void Simulation::initializeHolarchy(int levels, int holons)
{
    if(root)
        root->deleteLater();

    root = new Agent(NULL);
    root->setMaxFutileCycles(ui->maxCycles_lineEdit->text().toInt());
    root->setDesiredVariance(ui->desiredVariance_lineEdit->text().toDouble());

    connect(root, &Agent::simulationFinished, this, &Simulation::onSimulationFinished);
    connect(root, &Agent::peakLoadChanged, this, &Simulation::setPeakLoads);

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


        if(level==maxLevels-1)//set values for leafs
        {

            QVector<ushort> resources;
            QVector<double> priorities;

            for(int i=0; i<ResourceElements; i++)
            {
                double val;
                statistics.gaussianRandomGererator(meanResources[i], resourceStandardDeviation, 1, &val);
                resources.append((ushort)val);
                statistics.gaussianRandomGererator(meanPriorities[i], priorityStandardDeviation, 1, &val);
                priorities.append(val);
            }

            agent->setResources(resources);
            agent->setPriorities(priorities);
        }

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


    peakLoadPlotTimer.start();

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

    peakLoadPlotTimer.stop();
}

void Simulation::setPeakLoads(QVector<ushort> peakLoads)
{
    this->peakLoads = peakLoads;
}

void Simulation::updatePeakLoadPlot()
{
    double * xdata=new double[ResourceElements];
    double * ydata=new double[ResourceElements];

    for(int i=0;i<ResourceElements;i++)
    {
       xdata[i]=i+1;
       ydata[i]=peakLoads[i];
    }

    ui->peakLoadPlot->updateGraph(0, xdata, ydata, ResourceElements);

    delete xdata;
    delete ydata;
}
