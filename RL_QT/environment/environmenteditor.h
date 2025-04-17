#ifndef ENVIRONMENTEDITOR_H
#define ENVIRONMENTEDITOR_H

#include "cellitem.h"
#include "agent/agentobj.h"

class EnvironmentEditor
{
public:
    EnvironmentEditor();
    void change_cells(QList<CellItem*>& selected_cells, CellType type);
    void add_agents(QList<CellItem*>& selected_cells, QList<AgentObj*>& selected_agents, QList<AgentObj*>& all_agents, AgentType type);

private:
    void change_agent(QList<AgentObj*>& selected_agents, QList<AgentObj *>& all_agents, AgentType type);
};

#endif // ENVIRONMENTEDITOR_H
