#ifndef CELLFACTORY_H
#define CELLFACTORY_H

#include "cellitem.h"

#include <QGraphicsScene>
#include <memory>

class CellFactory
{
public:
    CellFactory();
    static std::unique_ptr<CellItem> create_cell(CellType type, QGraphicsItem *parent = nullptr);
    template<typename T> static CellItem* convert_cell(CellItem *old_cell)
    {
        auto new_cell = std::make_unique<T>(old_cell->parentItem());
        new_cell->setPos(old_cell->pos());

        if (old_cell->is_selected())
            new_cell->set_selected(true, false);

        if (auto scene = old_cell->scene())
        {
            scene->removeItem(old_cell);
            T* ptr = new_cell.get();
            scene->addItem(new_cell.release());
            return ptr;
        }

        return nullptr;
    }
};

#endif // CELLFACTORY_H
