#include "agent.h"

Agent::Agent(QObject *parent) : QObject(parent)
{
    m_parent = qobject_cast<Agent *>(parent);

    m_resources.resize(ResourceElements);
    m_thresholds.resize(ResourceElements);
    m_permissions.resize(ResourceElements);

    m_receivedSuggestionsFromChilds =0;


}

Agent::~Agent()
{
    foreach(Agent * agent, m_children)
    {
        agent->deleteLater();
    }
}

void Agent::addChild(Agent *agent)
{
    agent->setHolonIndex(m_children.count()+1);

    connect(agent, &Agent::suggestParent, this, &Agent::receiveSuggestFromChild);
    connect(agent, &Agent::suggestSibling, this, &Agent::mapChildrenSuggestions);

    m_children.append(agent);
}

QList<Agent *> Agent::children() const
{
    return m_children;
}

QVector<bool> Agent::permissions() const
{
    return m_permissions;
}

void Agent::start()
{
    if(m_children.isEmpty()) // This is a leaf
    {
        emit suggestParent(m_resources);
        return;
    }

    foreach(Agent * agent, m_children)
    {
        agent->start();
    }
}

void Agent::receiveSuggestFromChild(QVector<ushort> resources)
{
    m_receivedSuggestionsFromChilds++;

    for(int i=0;i< ResourceElements; i++)
        m_resources[i] += resources[i];

    if(m_receivedSuggestionsFromChilds == m_children.count())
    {
        if(m_parent==NULL) // This is root
        {
            sendResultToChildren(false);
        }
        else // This is an intermediate holon
        {
            emit suggestParent(m_resources);
        }

        m_receivedSuggestionsFromChilds = 0;

        for(int i=0;i< ResourceElements; i++)
             m_resources[i] =0 ;
    }
}

int Agent::holonIndex() const
{
    return m_holonIndex;
}

void Agent::setHolonIndex(int holonIndex)
{
    m_holonIndex = holonIndex;
}


void Agent::receiveResultFromParent(QVector<bool> permissions, QVector<ushort> thresholds)
{

    m_permissions = permissions;
    m_thresholds = thresholds;
}

void Agent::sendResultToChildren(bool finished)
{
    if(m_children.isEmpty()) //leaf
    {
        if(!finished)
            emit suggestParent(m_resources);

        return;
    }

    //Sending results to children
    QVector<ushort> meanThresholds;
    meanThresholds.resize(ResourceElements);
    for(int i=0;i< ResourceElements; i++)
    {
        meanThresholds[i] = m_thresholds[i] / m_children.count();
    }

    bool isFinished = true;
    foreach(Agent * agent, m_children)
    {
        for(int i=0;i< ResourceElements; i++)
        {
            if(agent->m_resources[i]<=meanThresholds[i])
                m_permissions[i] = true;
            else
                isFinished = m_permissions[i] = false;
        }


        agent->receiveResultFromParent(m_permissions, meanThresholds);
    }

    //Interaction
    if(!isFinished)
    {
        foreach(Agent * agent, m_children)
        {
            agent->interactWithSiblings();
        }
    }

    //Invoke children methods
    foreach(Agent * agent, m_children)
    {
        agent->sendResultToChildren(isFinished);
    }


}

void Agent::interactWithSiblings()
{
    foreach(Agent * agent, m_parent->children())
    {
        int givingIndex=-1;
        int gettingIndex=-1;
        QVector<bool> agentPermissions = agent->permissions();
        for(int i=0; i<ResourceElements;i++)
        {
            if(m_permissions[i]==false && agentPermissions[i]==true)
                gettingIndex = i;
            else if(m_permissions[i]==true && agentPermissions[i]==false)
                givingIndex = i;

            if(givingIndex>=0 && gettingIndex>=0)
                break;

        }
        if(givingIndex>=0 && gettingIndex>=0)
        {
            int exchangeAmount = 1;
            QVector<ushort> givingResources;
            givingResources.insert(0, ResourceElements, 0);
            givingResources[givingIndex] = exchangeAmount;

            QVector<ushort> gettingResources;
            gettingResources.insert(0, ResourceElements, 0);
            gettingResources[gettingIndex] = exchangeAmount;

            bool ret = suggestSibling(agent->holonIndex(), givingResources, gettingResources);

            if(ret)
            {
                shiftResource(givingIndex, exchangeAmount);
            }
        }
    }
}

bool Agent::receiveSuggestFromSibling(QVector<ushort> givingResources, QVector<ushort> gettingResources)
{
    int givingIndex=-1;
    int gettingIndex=-1;
    ushort exchangeAmount;
    for(int i=0; i<ResourceElements;i++)
    {
        if(givingResources[i]>0)
            givingIndex = i;
        else if(gettingResources[i]>0)
        {
            gettingIndex = i;
            exchangeAmount = gettingResources[i];
        }
    }

    if(givingIndex>=0 && gettingIndex>=0)
    {
        //decide

        shiftResource(gettingIndex, exchangeAmount);

        return true;

    }

    return false;
}

void Agent::shiftResource(int givingIndex, ushort exchangeAmount)
{
    m_resources[givingIndex]-= exchangeAmount;
    int diff=1;
    while(true)
    {
        if((givingIndex+diff)<ResourceElements && m_permissions[givingIndex+diff])
        {
            m_resources[givingIndex+diff]+=exchangeAmount;
            break;
        }
        else if((givingIndex-diff)>=0 && m_permissions[givingIndex-diff])
        {
            m_resources[givingIndex-diff]+=exchangeAmount;
            break;
        }
        else if((givingIndex+diff)>=ResourceElements && (givingIndex-diff)<0)
        {
            m_resources[0]+=exchangeAmount;
            break;
        }
        diff++;
    }
}

bool Agent::mapChildrenSuggestions(int targetIndex,  QVector<ushort> givingResources, QVector<ushort> gettingResources)
{
    //int sourceIndex = qobject_cast<Agent *>(sender())->HolonIndex;
    return m_children[targetIndex]->receiveSuggestFromSibling(givingResources, gettingResources);
}


