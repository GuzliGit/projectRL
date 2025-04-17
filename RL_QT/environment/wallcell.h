#ifndef WALLCELL_H
#define WALLCELL_H

#include "cellitem.h"

class WallCell : public CellItem
{
    Q_OBJECT
public:
    WallCell(QGraphicsItem *parent = nullptr);
    virtual CellType get_type() const override { return CellType::Wall; }
    virtual void update_cell_appearance() override;
    virtual bool is_walkable() const override { return false; }
};

#endif // WALLCELL_H
