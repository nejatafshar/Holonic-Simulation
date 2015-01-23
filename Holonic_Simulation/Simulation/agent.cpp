#include "agent.h"

#include "statistics.h"

Agent::Agent(QObject *parent) : QObject(parent)
{
    m_parent = qobject_cast<Agent *>(parent);

    m_permission.resize(ResourceElements);
    m_resources.resize(ResourceElements);
    m_priorities.resize(ResourceElements);

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

    connect(agent, &Agent::suggestSibling, [this](int targetHolonIndex, int gettingIndex, int givingIndex)->bool
    {
        //int sourceIndex = qobject_cast<Agent *>(sender())->holonIndex();
        return this->m_children[targetHolonIndex]->receiveSuggestFromSibling(gettingIndex, givingIndex);
    });

    connect(this, &Agent::sendResultToChild, agent, [this](int targetHolonIndex, QVector<bool> permissions)->void
    {
        return this->m_children[targetHolonIndex]->receiveResultFromParent(permissions);
    });

    connect(this, &Agent::sendInteractCommandToChilds, agent, &Agent::interactWithSiblings);
    connect(this, &Agent::sendContinueCommandToChilds, agent, &Agent::continueDownwards);


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
        emit suggestParent(m_resources, m_priorities);
        return;
    }

    foreach(Agent * agent, m_children)
    {
        agent->start();
    }
}

void Agent::receiveSuggestFromChild(QVector<ushort> resources, QVector<double> priorities)
{
    m_receivedSuggestionsFromChilds++;

    for(int i=0;i< ResourceElements; i++)
        m_resources[i] += resources[i];

    for(int i=0;i< ResourceElements; i++)
        m_priorities[i] += priorities[i];

    if(m_receivedSuggestionsFromChilds == m_children.count())
    {
        if(m_parent==NULL) // This is root
        {
            emit peakLoadChanged(m_resources);

            Statistics s;
            double r[ResourceElements];
            for(int i=0;i<ResourceElements;i++)
                r[i] = m_resources[i];
            double variance = s.getVariance(r, ResourceElements);

            if(variance<=m_desiredVariance)
            {
                emit simulationFinished();
            }
            else
                continueDownwards();

        }
        else // This is an intermediate holon
        {
            emit suggestParent(m_resources, m_priorities);
        }

        m_receivedSuggestionsFromChilds = 0;
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


void Agent::receiveResultFromParent(QVector<bool> permissions)
{
    m_permission = permissions;
}

void Agent::continueDownwards()
{

    if(!m_permission.contains(false)) //go up
    {
        emit suggestParent(m_resources, m_priorities);
        return;
    }
    else if(m_children.isEmpty()) //leaf
    {
        //...
        emit suggestParent(m_resources, m_priorities);
        return;
    }


    //Deciding about permissions and sending results to children
    for(int i=0;i<m_children.count();i++)
    {
        //...
        emit sendResultToChild(i, m_permission);
    }

    for(int i=0;i<ResourceElements;i++)
    {
        m_resources[i]=0;
        m_priorities[i]=0;
        m_receivedSuggestionsFromChilds = 0;
    }

    //Interaction
    emit sendInteractCommandToChilds();

    //Continue downwards
    emit sendContinueCommandToChilds();

}

void Agent::interactWithSiblings()
{
    int gettingIndex = m_permission.indexOf(false);

    QVector<double>::iterator it = std::max_element(m_priorities.begin(), m_priorities.end());
    double maxPriorityValue = *it;
    int maxPriorityIndex = m_priorities.indexOf(maxPriorityValue);

    if(maxPriorityIndex!=gettingIndex)
        return;


    for(int i=0;i<ResourceElements;i++)
    {

        if(i!=gettingIndex)
        {
            QVector<ushort> resources;
            foreach(Agent * agent, m_parent->children())
            {
                resources.append(agent->resources()[i]);
            }
            QVector<ushort>::iterator it = std::max_element(resources.begin(), resources.end());
            ushort maxValue = *it;
            int agentIndex = resources.indexOf(maxValue);

            if(agentIndex != m_holonIndex)
            {
                bool result = suggestSibling(agentIndex, gettingIndex, i);
                if(result)
                {
                    m_permission[gettingIndex] = true;
                    m_permission[i] = false;

                    return;
                }
            }
        }

    }


}

bool Agent::receiveSuggestFromSibling(int givingIndex,int gettingIndex)
{
    QVector<double>::iterator it = std::max_element(m_priorities.begin(), m_priorities.end());
    double maxPriorityValue = *it;
    int maxPriorityIndex = m_priorities.indexOf(maxPriorityValue);

    if(maxPriorityIndex==gettingIndex)
        return true;
    else
        return false;
}

void Agent::shiftResource(int givingIndex)
{
    ushort exchangeAmount = 1;

    m_resources[givingIndex]-= exchangeAmount;
    int diff=1;
    while(true)
    {
        if((givingIndex+diff)<ResourceElements && m_resources[givingIndex+diff]<m_resources[givingIndex])
        {
            m_resources[givingIndex+diff]+=exchangeAmount;
            break;
        }
        else if((givingIndex-diff)>=0 && m_resources[givingIndex+diff]<m_resources[givingIndex])
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
QVector<double> Agent::priorities() const
{
    return m_priorities;
}

void Agent::setPriorities(const QVector<double> &priorities)
{
    m_priorities = priorities;
}

QVector<bool> Agent::permissions() const
{
    return m_permission;
}

void Agent::setPermissions(const QVector<bool> &permissions)
{
    m_permission = permissions;
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



