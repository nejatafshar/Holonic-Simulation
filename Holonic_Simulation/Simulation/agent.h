#ifndef AGENT_H
#define AGENT_H

#include <QObject>

#include <QVector>

#define ResourceElements 24

class Agent : public QObject
{
    Q_OBJECT
public:
    explicit Agent(QObject *parent = 0);
    ~Agent();

signals:

    void suggestParent(QVector<ushort> resources);
    bool suggestSibling(int targetIndex, QVector<ushort> resources);


public slots:




private slots:

    void receiveSuggestFromChild(QVector<ushort> resources);

    bool mapChildrenSuggestions(int targetIndex, QVector<ushort> resources);

public :

    void addChild(Agent * agent);

    void start();

    void receiveResultFromParent(QVector<bool> permissions, QVector<ushort> thresholds);
    void sendResultToChildren(bool finished);
    void interactWithSiblings();

    bool receiveSuggestFromSibling(QVector<ushort> resources);

private:

    int HolonIndex;

    QList<Agent *> childs;
    Agent * parent;

    QVector<ushort> resources;
    QVector<ushort> thresholds;
    QVector<bool> permissions;

    int receivedSuggestionsFromChilds;

};

#endif // AGENT_H
