#ifndef ENVIRONMENTEDITOR_H
#define ENVIRONMENTEDITOR_H

#include "cellitem.h"
#include "agent/agentobj.h"

class EnvironmentEditor
{
public:
    EnvironmentEditor();
    void change_cells(QList<CellItem*>& selected_cells, CellType type);
    bool add_agent(QList<CellItem*>& selected_cells, AgentType type);
    void change_agent(QList<AgentObj*>& selected_agents, AgentType type);
};

#endif // ENVIRONMENTEDITOR_H
