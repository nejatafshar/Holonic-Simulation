#ifndef SIMULATION_H
#define SIMULATION_H

#include "Simulation_global.h"

#include <QObject>

#include <QWidget>

#include "agent.h"


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

private:

    void initializeHolarchy(int levels, int holons);

    void initializeHolon(Agent* parent, int holons, int level, int maxLevel);



private:
     Ui::Simulation *ui;

     Agent * root;



};

#endif // SIMULATION_H
