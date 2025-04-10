#include "environmenteditor.h"
#include "floorcell.h"
#include "wallcell.h"
#include "cellfactory.h"

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

bool EnvironmentEditor::add_agent(QList<CellItem*>& selected_cells, AgentType type)
{
    CellItem* cell = selected_cells.first();
    if (cell->get_type() == CellType::Empty || cell->get_type() == CellType::Wall)
        return false;

    switch (type){
    case AgentType::LimitedView:
        AgentObj* agent = new AgentObj();
        agent->setPos(cell->pos());
        cell->scene()->addItem(agent);
        return true;
        break;
    }

    return false;
}
