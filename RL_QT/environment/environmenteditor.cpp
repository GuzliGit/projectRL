#include "environmenteditor.h"
#include "floorcell.h"
#include "wallcell.h"
#include "cellfactory.h"
#include "agent/agentfactory.h"

#include <QGraphicsScene>

EnvironmentEditor::EnvironmentEditor() {}

void EnvironmentEditor::change_cells(QList<CellItem*>& selected_cells, CellType type)
{
    for (int i = 0; i < selected_cells.size(); i++)
    {
        CellItem* old_item = selected_cells[i];
        CellItem* new_item = nullptr;

        switch (type) {
        case CellType::Empty:
            new_item = CellFactory::convert_cell<CellItem>(old_item);
            break;
        case CellType::Floor:
            new_item = CellFactory::convert_cell<FloorCell>(old_item);
            break;
        case CellType::Wall:
            new_item = CellFactory::convert_cell<WallCell>(old_item);
            break;
        }

        if (new_item)
            selected_cells[i] = new_item;
    }

    if (selected_cells.empty())
        return;

    QPointF top_neighbour;
    QPointF bot_neighbour;

    for (auto item : selected_cells)
    {
        top_neighbour = QPointF(item->pos().x(), item->pos().y() - item->sceneBoundingRect().height() + 1);
        bot_neighbour = QPointF(item->pos().x(), item->pos().y() + item->sceneBoundingRect().height());
        item->update_cell_appearance();

        for (auto cell : item->scene()->items(top_neighbour))
        {
            if (auto temp = dynamic_cast<CellItem*>(cell))
                temp->update_cell_appearance();
        }

        for (auto cell : item->scene()->items(bot_neighbour))
        {
            if (auto temp = dynamic_cast<CellItem*>(cell))
                temp->update_cell_appearance();
        }
    }
}

void EnvironmentEditor::add_agents(QList<CellItem*>& selected_cells, QList<AgentObj*>& selected_agents, QList<AgentObj*>& all_agents, AgentType type)
{
    for (auto cell : selected_cells)
    {
        bool is_already_on_scene = false;

        if (!cell->is_walkable())
            continue;

        for (auto agent : all_agents)
        {
            if (cell->pos() == agent->pos() && type != agent->get_type())
            {
                selected_agents.removeOne(agent);
                all_agents.removeOne(agent);
                cell->scene()->removeItem(agent);
            }
            else if (cell->pos() == agent->pos() && type == agent->get_type())
                is_already_on_scene = true;
        }

        if (is_already_on_scene)
            continue;

        switch (type){
        case AgentType::LimitedView:
            AgentObj* agent = new AgentObj();
            agent->setPos(cell->pos());
            all_agents.append(agent);
            cell->scene()->addItem(agent);
            break;
        }
    }

    if (selected_agents.size() > 0)
        change_agent(selected_agents, all_agents, type);
}

void EnvironmentEditor::change_agent(QList<AgentObj *>& selected_agents, QList<AgentObj *>& all_agents, AgentType type)
{
    for (int i = 0; i < selected_agents.size(); i++)
    {
        AgentObj* old_item = selected_agents[i];
        AgentObj* new_item = nullptr;

        if (old_item->get_type() == type)
            continue;

        switch (type) {
        case AgentType::LimitedView:
            new_item = AgentFactory::convert_agent<AgentObj>(old_item);
            break;
        }

        if (new_item)
        {
            selected_agents[i] = new_item;
            all_agents.removeOne(old_item);
            all_agents.append(new_item);
        }
    }
}
