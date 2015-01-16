#include "agent.h"

Agent::Agent(QObject *parent) : QObject(parent)
{
    m_parent = qobject_cast<Agent *>(parent);

    m_prices.resize(ResourceElements);

    m_receivedSuggestionsFromChilds =0;

    m_maxFutileCycles = 0;
    m_desiredVariance = 0;


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
    agent->setHolonIndex(m_children.count());

    connect(agent, &Agent::suggestParent, this, &Agent::receiveSuggestFromChild);
    connect(agent, &Agent::suggestSibling, this, &Agent::mapChildrenSuggestions);

    m_children.append(agent);
}

QList<Agent *> Agent::children() const
{
    return m_children;
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
            bool isFinished = false;

            //

            if(isFinished)
                emit simulationFinished();

            sendResultToChildren(isFinished);

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


void Agent::receiveResultFromParent(QVector<double> prices)
{
    m_prices = prices;
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

    //Deciding About prices
    //...

    foreach(Agent * agent, m_children)
    {
        agent->receiveResultFromParent(m_prices);
    }

    //Interaction
    if(!finished)
    {
        foreach(Agent * agent, m_children)
        {
            agent->interactWithSiblings();
        }
    }


    //Invoke children methods
    foreach(Agent * agent, m_children)
    {
        agent->sendResultToChildren(finished);
    }


}

void Agent::interactWithSiblings()
{
    QVector<ushort>::iterator it = std::max_element(m_resources.begin(), m_resources.end());
    ushort max = *it;

    foreach(Agent * agent, m_parent->children())
    {
        QVector<ushort> agentResources = agent->resources();
        QVector<ushort>::iterator it = std::max_element(agentResources.begin(), agentResources.end());
        ushort agentMax = *it;

        // Decide about interaction
        if(false)
        {
            int givingIndex = m_resources.indexOf(max);
            int gettingIndex = agentResources.indexOf(agentMax);

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
        if((givingIndex+diff)<ResourceElements && m_prices[givingIndex+diff])
        {
            m_resources[givingIndex+diff]+=exchangeAmount;
            break;
        }
        else if((givingIndex-diff)>=0 && m_prices[givingIndex-diff])
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
QVector<ushort> Agent::priorities() const
{
    return m_priorities;
}

void Agent::setPriorities(const QVector<ushort> &priorities)
{
    m_priorities = priorities;
}

QVector<double> Agent::prices() const
{
    return m_prices;
}

void Agent::setPrices(const QVector<double> &prices)
{
    m_prices = prices;
}

QVector<ushort> Agent::resources() const
{
    return m_resources;
}

void Agent::setResources(const QVector<ushort> &resources)
{
    m_resources = resources;
}

int Agent::maxFutileCycles() const
{
    return m_maxFutileCycles;
}

void Agent::setMaxFutileCycles(int value)
{
    m_maxFutileCycles = value;
}

double Agent::desiredVariance() const
{
    return m_desiredVariance;
}

void Agent::setDesiredVariance(double desiredVariance)
{
    m_desiredVariance = desiredVariance;
}


bool Agent::mapChildrenSuggestions(int targetIndex,  QVector<ushort> givingResources, QVector<ushort> gettingResources)
{
    //int sourceIndex = qobject_cast<Agent *>(sender())->HolonIndex;
    return m_children[targetIndex]->receiveSuggestFromSibling(givingResources, gettingResources);
}



