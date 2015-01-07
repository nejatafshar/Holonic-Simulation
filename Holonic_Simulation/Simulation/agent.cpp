#include "agent.h"

Agent::Agent(QObject *parent) : QObject(parent)
{
    this->parent = qobject_cast<Agent *>(parent);

    resources.reserve(ResourceElements);

    receivedSuggestionsFromChilds =0;

    permissions.reserve(ResourceElements);

}

Agent::~Agent()
{
    foreach(Agent * agent, childs)
    {
        agent->deleteLater();
    }
}

void Agent::addChild(Agent *agent)
{
    agent->HolonIndex = childs.count()+1;

    connect(agent, &Agent::suggestParent, this, &Agent::receiveSuggestFromChild);
    connect(agent, &Agent::suggestSibling, this, &Agent::mapChildrenSuggestions);

    childs.append(agent);
}

void Agent::start()
{
    if(childs.isEmpty()) // This is a leaf
    {
        emit suggestParent(resources);
        return;
    }

    foreach(Agent * agent, childs)
    {
        agent->start();
    }
}

void Agent::receiveSuggestFromChild(QVector<ushort> resources)
{
    receivedSuggestionsFromChilds++;

    for(int i=0;i< ResourceElements; i++)
        this->resources[i] += resources[i];

    if(receivedSuggestionsFromChilds == childs.count())
    {
        if(parent==NULL) // This is root
        {
            sendResultToChildren(false);
        }
        else // This is an intermediate holon
        {
            emit suggestParent(this->resources);
        }

        receivedSuggestionsFromChilds = 0;

        for(int i=0;i< ResourceElements; i++)
             this->resources[i] =0 ;
    }
}

bool Agent::receiveSuggestFromSibling(QVector<ushort> resources)
{

    return false;
}

void Agent::receiveResultFromParent(QVector<bool> permissions, QVector<ushort> thresholds)
{

    this->permissions = permissions;
    this->thresholds = thresholds;
}

void Agent::sendResultToChildren(bool finished)
{
    if(childs.isEmpty()) //leaf
    {
        if(!finished)
            emit suggestParent(resources);
    }

    //Sending results to children
    QVector<ushort> meanThresholds;
    meanThresholds.reserve(ResourceElements);
    for(int i=0;i< ResourceElements; i++)
    {
        meanThresholds[i] = this->thresholds[i] / childs.count();
    }

    bool isFinished = true;
    foreach(Agent * agent, childs)
    {
        for(int i=0;i< ResourceElements; i++)
        {
            if(agent->resources[i]<=meanThresholds[i])
                this->permissions[i] = true;
            else
                isFinished = this->permissions[i] = false;
        }


        agent->receiveResultFromParent(this->permissions, meanThresholds);
    }

    //Interaction
    if(!isFinished)
    {
        foreach(Agent * agent, childs)
        {
            agent->interactWithSiblings();
        }
    }

    //Invoke children methods
    foreach(Agent * agent, childs)
    {
        agent->sendResultToChildren(isFinished);
    }


}

void Agent::interactWithSiblings()
{

}

bool Agent::mapChildrenSuggestions(int targetIndex, QVector<ushort> resources)
{
    //int sourceIndex = qobject_cast<Agent *>(sender())->HolonIndex;
    return childs[targetIndex]->receiveSuggestFromSibling(resources);
}


