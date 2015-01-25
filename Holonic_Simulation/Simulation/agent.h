#ifndef AGENT_H
#define AGENT_H

#include <QObject>

#include <QVector>

#define ResourceElements 24

class Agent : public QObject
{
    Q_OBJECT

public:
    explicit Agent(QObject *m_parent = 0);
    ~Agent();

signals:

    void suggestParent(QVector<ushort> resources, QVector<double> priorities);
    bool suggestSibling(int targetHolonIndex, int gettingIndex, int givingIndex);
    void sendResultToChild(int targetHolonIndex, QVector<bool> permissions);
    void sendInteractCommandToChilds();
    void sendContinueCommandToChilds();

    void simulationFinished();
    void resultChanged(QVector<ushort> resources, double variance);

public slots:


    void receiveSuggestFromChild(QVector<ushort> resources, QVector<double> priorities);
    bool receiveSuggestFromSibling(int givingIndex,int gettingIndex);
    void receiveResultFromParent(QVector<bool> permissions);
    void interactWithSiblings();
    void continueDownwards();


private slots:



public :


    void addChild(Agent * agent);

    void start();


    //Setters & Getters

    QList<Agent *> children() const;

    int holonIndex() const;
    void setHolonIndex(int holonIndex);

    int maxFutileCycles() const;
    void setMaxFutileCycles(int value);

    double desiredVariance() const;
    void setDesiredVariance(double desiredVariance);

    QVector<ushort> resources() const;
    void setResources(const QVector<ushort> &resources);

    QVector<bool> permissions() const;
    void setPermissions(const QVector<bool> &permissions);

    QVector<double> priorities() const;
    void setPriorities(const QVector<double> &priorities);

    bool stopped() const;
    void setStopped(bool stopped);

    bool horizontalInteraction() const;
    void setHorizontalInteraction(bool horizontalInteraction);

private:

    int getAgentWithMaxInPosition(int position, QList<Agent *> agents);

private:

    void shiftResource(int givingIndex);

    int m_holonIndex;

    QList<Agent *> m_children;
    Agent * m_parent;

    QVector<bool> m_permission;
    QVector<ushort> m_resources;
    QVector<double> m_priorities;

    int m_receivedSuggestionsFromChilds;

    int m_maxFutileCycles;
    double m_desiredVariance;
    bool m_stopped;
    bool m_horizontalInteraction;

};

#endif // AGENT_H
