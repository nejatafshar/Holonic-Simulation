#include "agent.h"

Agent::Agent(QObject *parent) : QObject(parent)
{
    this->parent = qobject_cast<Agent *>(parent);

}

Agent::~Agent()
{

}

