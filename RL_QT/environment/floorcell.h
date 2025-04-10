#ifndef FLOORCELL_H
#define FLOORCELL_H

#include "cellitem.h"

class FloorCell : public CellItem
{
    Q_OBJECT
public:
    FloorCell(QGraphicsItem *parent);
    virtual CellType get_type() const override { return CellType::Floor; }
    virtual void update_cell_appearance() override;
    virtual bool is_walkable() const override { return true; }
};

#endif // FLOORCELL_H
