#ifndef CELLITEM_H
#define CELLITEM_H

#include <QGraphicsPixmapItem>

class CellItem : public QGraphicsPixmapItem
{
public:
    CellItem(QGraphicsItem *parent = nullptr);
    int get_width();
    int get_height();

private:
    int width;
    int height;
};

#endif // CELLITEM_H
