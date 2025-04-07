#include "floorcell.h"
#include "cellitem.h"
#include "wallcell.h"

#include <QGraphicsScene>

FloorCell::FloorCell(QGraphicsItem *parent) : CellItem(parent)
{
    setPixmap(QPixmap(":/img/img/FloorCell.svg"));
}

void FloorCell::update_cell_appearance()
{
    bool is_wall_above = false;
    QPointF upper_pos(pos().x(), pos().y() - boundingRect().height());

    for (auto item : scene()->items(upper_pos))
    {
        if (dynamic_cast<WallCell*>(item))
        {
            is_wall_above = true;
            break;
        }
    }

    setPixmap(QPixmap(is_wall_above ? ":/img/img/FloorWithShadow.svg" : ":/img/img/FloorCell.svg"));
}
