#include "cellfactory.h"
#include "cellitem.h"
#include "floorcell.h"
#include "wallcell.h"

#include <QGraphicsScene>

CellFactory::CellFactory() {}

std::unique_ptr<CellItem> CellFactory::create_cell(CellType type, QGraphicsItem *parent)
{
    switch (type) {
    case CellType::Empty:
        return std::make_unique<CellItem>(parent);
        break;
    case CellType::Floor:
        return std::make_unique<FloorCell>(parent);
        break;
    case CellType::Wall:
        return std::make_unique<WallCell>(parent);
        break;
    }
}
