#include "wallcell.h"
#include "cellitem.h"

#include <QGraphicsScene>

WallCell::WallCell(QGraphicsItem *parent) : CellItem(parent)
{
    setPixmap(QPixmap(":/img/img/WallWithBottom.svg"));
}

void WallCell::update_cell_appearance()
{
    bool is_wall_below = false;
    QPointF bottom_pos(pos().x(), pos().y() + boundingRect().height());

    for (auto item : scene()->items(bottom_pos))
    {
        if (dynamic_cast<WallCell*>(item))
        {
            is_wall_below = true;
            break;
        }
    }

    setPixmap(QPixmap(is_wall_below ? ":/img/img/SolidWall.svg" : ":/img/img/WallWithBottom.svg"));
}
