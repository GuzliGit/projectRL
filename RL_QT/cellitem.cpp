#include "cellitem.h"

CellItem::CellItem(QGraphicsItem *parent)
{
    QPixmap pixmap(":/img/img/EmptyCell.svg");
    width = pixmap.width();
    height = pixmap.height();

    setPixmap(pixmap);

    setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable, false);
}

int CellItem::get_width()
{
    return width;
}

int CellItem::get_height()
{
    return height;
}
