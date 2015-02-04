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
    variance = 0;

    QSettings settings;
    ui->levels_lineEdit->setText(settings.value("SimulationSettings/levels","2").toString());
    ui->holons_lineEdit->setText(settings.value("SimulationSettings/holons","20").toString());
    ui->maxCycles_lineEdit->setText(settings.value("SimulationSettings/maxCycles","4000").toString());
    ui->desiredVariance_lineEdit->setText(settings.value("SimulationSettings/desiredVariance","1").toString());
    ui->agentNeeds_lineEdit->setText(settings.value("SimulationSettings/agentNeeds","100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100").toString());
    ui->resourceStandardDeviation_lineEdit->setText(settings.value("SimulationSettings/standardDeviation","4").toString());
    ui->priorities_lineEdit->setText(settings.value("SimulationSettings/priorities","50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50").toString());
    ui->prioritiesStandardDeviation_lineEdit->setText(settings.value("SimulationSettings/prioritiesStandardDeviation","4").toString());

    initializePlot();

    peakLoadPlotTimer.setInterval(30);
    connect(&peakLoadPlotTimer, &QTimer::timeout, this, &Simulation::updateResults );


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

        plot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 10));

        QPen graphPen = plot->graph(0)->pen();
        graphPen.setWidth(5);
        plot->graph(0)->setPen(graphPen);

        plot->yAxis->setSubTickCount(0);

        plot->autoRescale=true;
        plot->rescaleYWithMargin=true;
        plot->rescaleXWithMargin=true;
        plot->XmarginFactor = 0.04;
        plot->holdMaxScale = true;
        plot->holdMaxScaleFactor = 0.3;
        plot->holdMinScale = true;
        plot->holdMinScaleFactor = 0.3;
        plot->maxIndicateNum = 1;
        plot->minIndicateNum = 1;
        plot->setHasZoomModeBut(false);

        plot->setButtonsTransparency(50);

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
    connect(root, &Agent::resultChanged, this, &Simulation::setResults);


    //Initialize variables
    meanResources.clear();
    QStringList list1 = ui->agentNeeds_lineEdit->text().split(",");
    foreach(QString item, list1)
    {
        meanResources.append(item.toUInt());
        peakLoads[meanResources.count()-1] = 0;
    }
    resourceStandardDeviation = ui->resourceStandardDeviation_lineEdit->text().toDouble();

    meanPriorities.clear();
    QStringList list2 = ui->priorities_lineEdit->text().split(",");
    foreach(QString item, list2)
    {
        meanPriorities.append(item.toDouble());
    }
    priorityStandardDeviation = ui->prioritiesStandardDeviation_lineEdit->text().toDouble();

    //Make Holarchy
    initializeHolon(root, holons, 0, levels);

    root->setHorizontalInteraction(ui->horizontalInteractionChkBx->isChecked());
}

void Simulation::initializeHolon(Agent* parent, int holons, int level, int maxLevels)
{

    if(level == maxLevels)
        return;

    level++;

    for(int i=0;i<holons;i++)
    {
        Agent * agent = new Agent(parent);


        if(level==maxLevels)//set values for leafs
        {

            QVector<uint> resources;
            QVector<double> priorities;

            for(int i=0; i<ResourceElements; i++)
            {
                double val;
                statistics.gaussianRandomGererator(meanResources[i], resourceStandardDeviation, 1, &val);
                resources.append((uint)qAbs(val));
                peakLoads[i]+=resources[i];
                statistics.gaussianRandomGererator(meanPriorities[i], priorityStandardDeviation, 1, &val);
                priorities.append(qMin((double)qAbs(val),100.0));
            }

            agent->setResources(resources);
            agent->setPriorities(priorities);
        }

        parent->addChild(agent);
        initializeHolon(agent, holons, level, maxLevels);
    }
}

double Simulation::getSatisfactionRate(Agent *agent)
{
    if(!agent->children().isEmpty())
    {
        double sum=0;
        foreach(Agent * a,agent->children())
        {
            sum+=getSatisfactionRate(a);
        }
        return sum/agent->children().count();
    }
    else
    {
        QVector<uint> resources = agent->resources();
        QVector<double> priorities = agent->priorities();
        QVector<uint> primaryResources = agent->primaryResources();

        double sum1=0;
        double sum2=0;

        for(int i=0;i<ResourceElements;i++)
        {
            sum1+=(1.0+( ((double)qMin(0.0,(double)((double)resources[i]-(double)primaryResources[i])))/((double)primaryResources[i]) )) * priorities[i];
            sum2+=priorities[i];

            if(resources[i]!=primaryResources[i])
                int b =0;
        }

        return sum1/sum2;
    }
}

void Simulation::on_startBut_clicked()
{
    elapsedTimer.start();

    root->start();

    peakLoadPlotTimer.start();

    ui->stopBut->setEnabled(true);
    ui->startBut->setEnabled(false);
    ui->initializeHolarchyBut->setEnabled(false);

}

void Simulation::on_stopBut_clicked()
{
    root->setStopped(true);

    ui->stopBut->setEnabled(false);
    ui->startBut->setEnabled(false);
    ui->initializeHolarchyBut->setEnabled(true);
}

void Simulation::on_initializeHolarchyBut_clicked()
{
    int levels = ui->levels_lineEdit->text().toInt();
    int holons = ui->holons_lineEdit->text().toInt();

    initializeHolarchy(levels, holons);

    double r[ResourceElements];
    for(int i=0;i<ResourceElements;i++)
        r[i] = peakLoads[i];
    variance = statistics.getVariance(r, ResourceElements);

    verticalCycles = 0;

    elapsedTimer.start();


    ui->peakLoadPlot->holdMaxScale = false;
    ui->peakLoadPlot->holdMinScale = false;

    updateResults();

    ui->peakLoadPlot->holdMaxScale = true;
    ui->peakLoadPlot->holdMinScale = true;

    ui->startBut->setEnabled(true);
}

void Simulation::updateTotalHolons()
{
    int holons = ui->holons_lineEdit->text().toInt();
    int levels = ui->levels_lineEdit->text().toInt();
    ui->totalHolons_lineEdit->setText(QString("%1").arg(qPow(holons, levels)+holons+1));
}

void Simulation::onSimulationFinished()
{

    updateResults();

    //Update satisfaction rate
    double satisfactionRate = getSatisfactionRate(root)*100;
    ui->satisfactionRate_lineEdit->setText(QString::number(satisfactionRate,'2',2));

    peakLoadPlotTimer.stop();

    if(ui->stopBut->isEnabled())
        on_stopBut_clicked();
}

void Simulation::setResults(QVector<uint> peakLoads, double variance, int verticalCycles)
{
    this->peakLoads = peakLoads;
    this->variance = variance;
    this->verticalCycles = verticalCycles;
}

void Simulation::updateResults()
{
    //Update elapsed time
    qint64 elapsed = elapsedTimer.nsecsElapsed();
    ui->simulationTime_lineEdit->setText(QString::number(elapsed/1e9, 'f', 2));

    //Update peak load
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


    //Update peak load in lineEdit

    QStringList p;
    for(int i=0;i<peakLoads.count();i++)
        p.append(QString("%1").arg(peakLoads[i]));

    ui->peakLoad_lineEdit->setText(p.join(", "));


    //Update variance
    ui->variance_lineEdit->setText(QString::number(variance, 'f', 2));

    //Update vertical cycles
    ui->verticalCycles_lineEdit->setText(QString::number(verticalCycles));
}

void Simulation::on_horizontalInteractionChkBx_toggled(bool checked)
{
    if(root)
        root->setHorizontalInteraction(checked);
}
