#ifndef AGENTFACTORY_H
#define AGENTFACTORY_H

#include "agentobj.h"

#include <QGraphicsScene>
#include <memory>

class AgentFactory
{
public:
    AgentFactory();
    static std::unique_ptr<AgentObj> create_agent(AgentType type, QGraphicsItem *parent = nullptr);
    template<typename T> static AgentObj* convert_agent(AgentObj *old_agent)
    {
        auto new_agent = std::make_unique<T>(old_agent->parentItem());
        new_agent->setPos(old_agent->pos());

        if (old_agent->is_selected())
            new_agent->set_selected(true);

        if (auto scene = old_agent->scene())
        {
            scene->removeItem(old_agent);
            T* ptr = new_agent.get();
            scene->addItem(new_agent.release());
            return ptr;
        }

        return nullptr;
    }
};

#endif // AGENTFACTORY_H
