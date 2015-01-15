#ifndef SIMULATION_H
#define SIMULATION_H

#include "Simulation_global.h"

#include <QObject>
#include <QWidget>
#include <QVector>
#include <QElapsedTimer>

#include "agent.h"
#include "statistics.h"


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

    void updateTotalHolons();

    void onSimulationFinished();

private:

    void initializeHolarchy(int levels, int holons);

    void initializeHolon(Agent* parent, int holons, int level, int maxLevel);



private:
     Ui::Simulation *ui;

     Agent * root;


     QElapsedTimer elapsedTimer;

     Statistics statistics;

     QVector<ushort> meanResources;
     double resourceStandardDeviation;

     QVector<ushort> meanPriorities;
     double priorityStandardDeviation;

};

#endif // SIMULATION_H
