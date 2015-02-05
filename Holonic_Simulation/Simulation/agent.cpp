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
    m_bestVariance = INT_MAX;
    m_stopped = false;
    m_verticalCycles = 0;
    m_futileCycles = 0;


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

    connect(agent, &Agent::suggestParent, this, &Agent::receiveSuggestFromChild, Qt::QueuedConnection);

    connect(agent, &Agent::suggestSibling, [this](int targetHolonIndex, int gettingIndex, int givingIndex)->bool
    {
        //int sourceIndex = qobject_cast<Agent *>(sender())->holonIndex();
        return this->m_children[targetHolonIndex]->receiveSuggestFromSibling(gettingIndex, givingIndex);
    });

    connect(this, &Agent::sendResultToChild, agent, [this](int targetHolonIndex, QVector<bool> permissions)->void
    {
        return this->m_children[targetHolonIndex]->receiveResultFromParent(permissions);
    });

    connect(this, &Agent::sendMakePermissionsCommandToChilds, agent, &Agent::makePermissions);
    connect(this, &Agent::sendInteractCommandToChilds, agent, &Agent::interactWithSiblings);
    connect(this, &Agent::sendContinueCommandToChilds, agent, &Agent::continueDownwards, Qt::QueuedConnection);


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

void Agent::receiveSuggestFromChild(QVector<double> resources, QVector<double> priorities)
{
    m_receivedSuggestionsFromChilds++;

    for(int i=0;i< ResourceElements; i++)
        m_resources[i] += resources[i];

    for(int i=0;i< ResourceElements; i++)
        m_priorities[i] += priorities[i];

    if(m_receivedSuggestionsFromChilds == m_children.count())
    {
        for(int i=0;i< ResourceElements; i++)
            m_priorities[i] /= m_priorities.count();

        if(m_parent==NULL) // This is root
        {
            for(int i=0;i< ResourceElements; i++)
                m_priorities[i] /= m_priorities.count();

            m_verticalCycles++;

            Statistics s;
            double r[ResourceElements];
            for(int i=0;i<ResourceElements;i++)
                r[i] = m_resources[i];
            double variance = s.getVariance(r, ResourceElements);

            if(variance<m_bestVariance)
            {
                m_bestVariance = variance;
                m_futileCycles = 0;
            }
            else
                m_futileCycles++;

            emit resultChanged(m_resources, variance, m_verticalCycles);

            if(variance<=m_desiredVariance || m_stopped || m_futileCycles>=m_maxFutileCycles)
            {
                m_stopped = false;
                emit simulationFinished();
            }
            else
            {
                for(int i=0;i<ResourceElements;i++)
                    m_permission[i]=true;

                makePermissions();

                continueDownwards();
            }

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

void Agent::makePermissions()
{
    QVector<double>::iterator it = std::max_element(m_resources.begin(), m_resources.end());
    double maxResourceValue = *it;
    int maxResourceIndex = m_resources.indexOf(maxResourceValue);

    m_permission[maxResourceIndex] = false;
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
        for(int i=0; i<ResourceElements;i++)
        {
            if(m_permission[i]==false)
                shiftResource(i);
        }

        emit suggestParent(m_resources, m_priorities);
        return;
    }


    //Deciding about permissions and sending results to children
    int resourceIndex = m_permission.indexOf(false);
    int agentIndex = getAgentWithMaxInPosition(resourceIndex, m_children);
    for(int i=0;i<m_children.count();i++)
    {
        QVector<bool> permissions;
        permissions.insert(0, ResourceElements, true);
        if(i==agentIndex)
            permissions[resourceIndex] = false;

        emit sendResultToChild(i, permissions);
    }

    for(int i=0;i<ResourceElements;i++)
    {
        m_resources[i]=0;
        m_priorities[i]=0;
        m_receivedSuggestionsFromChilds = 0;
    }

    //make permissions

    emit sendMakePermissionsCommandToChilds();

    //Interaction
    if(m_horizontalInteraction)
        emit sendInteractCommandToChilds();

    //Continue downwards
    emit sendContinueCommandToChilds();

}

void Agent::interactWithSiblings()
{   

//    Statistics s;
//    double mean = s.getMean(m_priorities.data(), m_priorities.count());
//    if(m_priorities[gettingIndex]<=mean)
//        return;


    foreach(Agent * agent, m_parent->children())
    {
        if(agent!=this)
        {
            int gettingIndex=-1;
            int givingIndex=-1;

            QVector<bool> agentPermissions = agent->permissions();
            double maxPriority = 0;
            double minPriority = INT_MAX;
            for(int i=0; i<ResourceElements;i++)
            {
                if(m_permission[i]==false && agentPermissions[i]==true && m_priorities[i]>maxPriority)
                {
                    maxPriority = m_priorities[i];
                    gettingIndex = i;
                }
                else if(m_permission[i]==true && agentPermissions[i]==false && m_priorities[i]<minPriority)
                {
                    minPriority = m_priorities[i];
                    givingIndex = i;
                }
            }
            if(givingIndex>=0 && gettingIndex>=0)
            {
                bool result = suggestSibling(agent->holonIndex(), gettingIndex, givingIndex);

                if(result)
                {
                    m_permission[gettingIndex] = true;
                    m_permission[givingIndex] = false;

                    return;
                }
            }

        }

    }


}

bool Agent::receiveSuggestFromSibling(int givingIndex,int gettingIndex)
{
//    QVector<double>::iterator it = std::max_element(m_priorities.begin(), m_priorities.end());
//    double maxPriorityValue = *it;
//    int maxPriorityIndex = m_priorities.indexOf(maxPriorityValue);

//    if(maxPriorityIndex==gettingIndex)
//        return true;
//    else
//        return false;

    Statistics s;
    double mean = s.getMean(m_priorities.data(), m_priorities.count());
    if(m_priorities[givingIndex]<=m_priorities[gettingIndex])
    {
        m_priorities[givingIndex]=false;
        m_priorities[gettingIndex]=true;

        return true;
    }
    else
        return false;
}

void Agent::shiftResource(int givingIndex)
{
    double priorityRatio = (1.0-(((double)sortedPriorities.indexOf(m_priorities[givingIndex]))/((double)ResourceElements)));

//    if((m_primaryResources[givingIndex]-m_resources[givingIndex])>(0.5*priorityRatio*m_primaryResources[givingIndex]))
//        return;

    double exchangeAmount = priorityRatio*m_primaryResources[givingIndex];

    if((m_resources[givingIndex]-exchangeAmount)<0)
        return;
    else
        m_resources[givingIndex]-= exchangeAmount;

    int diff=1;
    while(true)
    {
        if( ((givingIndex+diff)<ResourceElements) && (m_resources[givingIndex+diff]<m_resources[givingIndex]) && (m_priorities[givingIndex+diff]>m_priorities[givingIndex]) )
        {
            m_resources[givingIndex+diff]+=exchangeAmount;
            break;
        }
        else if( ((givingIndex-diff)>=0) && (m_resources[givingIndex-diff]<m_resources[givingIndex]) && (m_priorities[givingIndex-diff]>m_priorities[givingIndex]) )
        {
            m_resources[givingIndex-diff]+=exchangeAmount;
            break;
        }
        else if((givingIndex+diff)>=ResourceElements && (givingIndex-diff)<0)
        {
            m_resources[(((double)qrand())/((double)RAND_MAX))*ResourceElements]+=exchangeAmount;
            break;
        }
        diff++;
    }
}
QVector<double> Agent::primaryResources() const
{
    return m_primaryResources;
}

void Agent::setPrimaryResources(const QVector<double> &primaryResources)
{
    m_primaryResources = primaryResources;
}

int Agent::verticalCycles() const
{
    return m_verticalCycles;
}

void Agent::setVerticalCycles(int verticalCycles)
{
    m_verticalCycles = verticalCycles;
}

double Agent::bestVariance() const
{
    return m_bestVariance;
}

void Agent::setBestVariance(double bestVariance)
{
    m_bestVariance = bestVariance;
}

bool Agent::horizontalInteraction() const
{
    return m_horizontalInteraction;
}

void Agent::setHorizontalInteraction(bool horizontalInteraction)
{
    m_horizontalInteraction = horizontalInteraction;

    foreach(Agent * agent, m_children)
        agent->setHorizontalInteraction(false);
}

bool Agent::stopped() const
{
    return m_stopped;
}

void Agent::setStopped(bool stopped)
{
    m_stopped = stopped;
}

QVector<double> Agent::priorities() const
{
    return m_priorities;
}

void Agent::setPriorities(const QVector<double> &priorities)
{
    m_priorities = priorities;

    sortedPriorities = m_priorities;
    std::sort(sortedPriorities.begin(), sortedPriorities.end(), std::less<double>());

}

int Agent::getAgentWithMaxInPosition(int position, QList<Agent *> agents)
{
    QVector<double> resources;
    foreach(Agent * agent, agents)
    {
        resources.append(agent->resources()[position]);
    }
    QVector<double>::iterator it = std::max_element(resources.begin(), resources.end());
    double maxValue = *it;
    return resources.indexOf(maxValue);
}

QVector<bool> Agent::permissions() const
{
    return m_permission;
}

void Agent::setPermissions(const QVector<bool> &permissions)
{
    m_permission = permissions;
}

QVector<double> Agent::resources() const
{
    return m_resources;
}

void Agent::setResources(const QVector<double> &resources)
{
    m_resources = resources;

    setPrimaryResources(resources);
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



