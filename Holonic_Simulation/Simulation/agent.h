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

    void suggestParent(QVector<ushort> resources);
    bool suggestSibling(int targetHolonIndex, QVector<ushort> givingResources, QVector<ushort> gettingResources);
    void simulationFinished();


public slots:




private slots:

    void receiveSuggestFromChild(QVector<ushort> resources);

    bool mapChildrenSuggestions(int targetHolonIndex,  QVector<ushort> givingResources, QVector<ushort> gettingResources);

public :

    QList<Agent *> children() const;

    int holonIndex() const;
    void setHolonIndex(int holonIndex);

    void addChild(Agent * agent);

    void start();

    void receiveResultFromParent(QVector<double> prices);
    void sendResultToChildren(bool finished);
    void interactWithSiblings();

    bool receiveSuggestFromSibling(QVector<ushort> givingResources, QVector<ushort> gettingResources);


    //Setters & Getters

    int maxFutileCycles() const;
    void setMaxFutileCycles(int value);

    double desiredVariance() const;
    void setDesiredVariance(double desiredVariance);


    QVector<ushort> resources() const;
    void setResources(const QVector<ushort> &resources);

    QVector<double> prices() const;
    void setPrices(const QVector<double> &prices);

    QVector<ushort> priorities() const;
    void setPriorities(const QVector<ushort> &priorities);

private:

    void shiftResource(int givingIndex, ushort exchangeAmount);

    int m_holonIndex;

    QList<Agent *> m_children;
    Agent * m_parent;

    QVector<double> m_prices;
    QVector<ushort> m_resources;
    QVector<ushort> m_priorities;

    int m_receivedSuggestionsFromChilds;

    int m_maxFutileCycles;
    double m_desiredVariance;

};

#endif // AGENT_H
