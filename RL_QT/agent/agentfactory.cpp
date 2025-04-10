#include "agentfactory.h"
#include "agentobj.h"

AgentFactory::AgentFactory() {}

std::unique_ptr<AgentObj> AgentFactory::create_agent(AgentType type, QGraphicsItem *parent)
{
    switch (type) {
    case AgentType::LimitedView:
        return std::make_unique<AgentObj>(parent);
        break;
    }
}
