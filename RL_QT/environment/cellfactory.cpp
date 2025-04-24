#include "cellfactory.h"
#include "cellitem.h"
#include "floorcell.h"
#include "wallcell.h"
#include "gatecell.h"
#include "riskycell.h"

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
    case CellType::Gate:
        return std::make_unique<GateCell>(parent);
        break;
    case CellType::Risky:
        return std::make_unique<RiskyCell>(parent);
        break;
    }
}
