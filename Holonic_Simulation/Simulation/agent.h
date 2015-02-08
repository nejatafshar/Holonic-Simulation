#ifndef AGENT_H
#define AGENT_H

#include <QObject>

#include <QVector>

#include "statistics.h"

#define ResourceElements 24

class Agent : public QObject
{
    Q_OBJECT

public:
    explicit Agent(QObject *m_parent = 0);
    ~Agent();

signals:

    void suggestParent(QVector<double> resources, QVector<double> priorities);
    bool suggestSibling(int targetHolonIndex, int gettingIndex, int givingIndex);
    void sendResultToChild(int targetHolonIndex, QVector<bool> permissions);
    void sendMakePermissionsCommandToChilds();
    void sendInteractCommandToChilds();
    void sendContinueCommandToChilds();

    void simulationFinished();
    void resultChanged(QVector<double> resources, double variance, int verticalCycles);

public slots:


    void receiveSuggestFromChild(QVector<double> resources, QVector<double> priorities);
    bool receiveSuggestFromSibling(int givingIndex,int gettingIndex);
    void receiveResultFromParent(QVector<bool> permissions);
    void makePermissions();
    void interactWithSiblings();
    void continueDownwards();


private slots:



public :


    void addChild(Agent * agent);

    void start();

    void reset();


    //Setters & Getters

    QList<Agent *> children() const;

    int holonIndex() const;
    void setHolonIndex(int holonIndex);

    int maxFutileCycles() const;
    void setMaxFutileCycles(int value);

    double desiredVariance() const;
    void setDesiredVariance(double desiredVariance);

    QVector<double> resources() const;
    void setResources(const QVector<double> &resources);

    QVector<bool> permissions() const;
    void setPermissions(const QVector<bool> &permissions);

    QVector<double> priorities() const;
    void setPriorities(const QVector<double> &priorities);

    bool stopped() const;
    void setStopped(bool stopped);

    bool horizontalInteraction() const;
    void setHorizontalInteraction(bool horizontalInteraction);

    double bestVariance() const;
    void setBestVariance(double bestVariance);

    int verticalCycles() const;
    void setVerticalCycles(int verticalCycles);

    QVector<double> primaryResources() const;
    void setPrimaryResources(const QVector<double> &primaryResources);

private:

    int getAgentWithMaxInPosition(int position, QList<Agent *> agents);

private:

    void shiftResource(int givingIndex);

    int m_holonIndex;

    QList<Agent *> m_children;
    Agent * m_parent;

    QVector<bool> m_permission;
    QVector<double> m_resources;
    QVector<double> m_priorities;

    QVector<double> sortedPriorities;
    QVector<double> m_primaryResources;

    int m_receivedSuggestionsFromChilds;

    int m_maxFutileCycles;
    double m_desiredVariance;
    double m_bestVariance;
    bool m_stopped;
    bool m_horizontalInteraction;
    int m_verticalCycles;
    int m_futileCycles;

    Statistics statistics;

};

#endif // AGENT_H
