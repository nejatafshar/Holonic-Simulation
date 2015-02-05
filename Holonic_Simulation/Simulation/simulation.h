#ifndef SIMULATION_H
#define SIMULATION_H

#include "Simulation_global.h"

#include <QObject>
#include <QWidget>
#include <QVector>
#include <QElapsedTimer>
#include <QTimer>

#include "agent.h"
#include "statistics.h"

#include <QCheckBox>


namespace Ui {
class Simulation;
}


class SIMULATIONSHARED_EXPORT Simulation : public QWidget
{
    Q_OBJECT
public:
    Simulation(QWidget *parent = 0);
    ~Simulation();






private slots:
    void on_startBut_clicked();
    void on_initializeHolarchyBut_clicked();

    void updateTotalHolons();

    void onSimulationFinished();
    void setResults(QVector<double> peakLoads, double variance, int verticalCycles);

    void updateResults();


    void on_stopBut_clicked();

    void on_horizontalInteractionChkBx_toggled(bool checked);

private:

    void initializePlot();

    void initializeHolarchy(int levels, int holons);

    void initializeHolon(Agent* parent, int holons, int level, int maxLevel);

    double getSatisfactionRate(Agent* agent);


private:
     Ui::Simulation *ui;

     Agent * root;

     QVector<double> peakLoads;
     double variance;
     int verticalCycles;
     QTimer peakLoadPlotTimer;

     QElapsedTimer elapsedTimer;

     Statistics statistics;

     QVector<double> meanResources;
     QVector<double> resourcesStandardDeviations;

     QVector<double> meanPriorities;
     QVector<double> prioritiesStandardDeviations;

     QCheckBox rescaleChkBx;

};

#endif // SIMULATION_H
