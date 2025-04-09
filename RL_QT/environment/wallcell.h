#ifndef WALLCELL_H
#define WALLCELL_H

#include "cellitem.h"

class WallCell : public CellItem
{
    Q_OBJECT
public:
    WallCell(QGraphicsItem *parent);
    virtual CellType get_type() const override { return CellType::Wall; }
    virtual void update_cell_appearance() override;
};

#endif // WALLCELL_H
