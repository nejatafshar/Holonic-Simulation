#ifndef AGENT_H
#define AGENT_H

#include <QObject>

class Agent : public QObject
{
    Q_OBJECT
public:
    explicit Agent(QObject *parent = 0);
    ~Agent();

signals:


public slots:

private:

    QList<Agent *> childs;
    Agent * parent;

};

#endif // AGENT_H
