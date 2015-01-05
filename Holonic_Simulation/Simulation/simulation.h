#ifndef SIMULATION_H
#define SIMULATION_H

#include "Simulation_global.h"

#include <QObject>

#include <QWidget>


namespace Ui {
class Simulation;
}


class SIMULATIONSHARED_EXPORT Simulation : public QWidget
{
    Q_OBJECT
public:
    Simulation(QWidget *parent = 0);
    ~Simulation();


signals:



public slots:

private slots:



private:
     Ui::Simulation *ui;



};

#endif // SIMULATION_H
