#include "gatecell.h"
#include "wallcell.h"

#include <random>
#include <QGraphicsScene>

GateCell::GateCell(QGraphicsItem *parent) : CellItem(parent)
{
    free_cell = QPixmap(":/img/img/GateCellFree.svg");
    free_cell_ws = QPixmap(":/img/img/GateCellFreeWithShadow.svg");
    blocked_cell = QPixmap(":/img/img/GateCellBlocked.svg");
    blocked_cell_ws = QPixmap(":/img/img/GateCellBlockedWithShadow.svg");

    setPixmap(QPixmap(":/img/img/GateCellFree.svg"));

    std::mt19937 rng_t(std::random_device{}());
    this->rng = rng_t;
    std::uniform_int_distribution<int> dist_t(0, 99);
    this->dist = dist_t;
}

void GateCell::update_cell_appearance()
{
    is_wall_above = false;
    QPointF upper_pos(pos().x(), pos().y() - sceneBoundingRect().height() + 1);

    for (auto item : scene()->items(upper_pos))
    {
        if (dynamic_cast<WallCell*>(item))
        {
            is_wall_above = true;
            break;
        }
    }

    if (is_blocked)
    {
        setPixmap(QPixmap(is_wall_above ? blocked_cell_ws : blocked_cell));
    }
    else
    {
        setPixmap(QPixmap(is_wall_above ? free_cell_ws : free_cell));
    }
}

int GateCell::get_closing_chance()
{
    return closing_chance;
}

void GateCell::set_closing_chance(int val)
{
    closing_chance = val;
}

void GateCell::update_status(bool has_agent)
{
    if (has_agent)
        return;

    int random_val = dist(rng);
    if (random_val < closing_chance)
    {
        if (!is_blocked)
            update_current_appearance();

        is_blocked = true;
    }
    else
    {
        if (is_blocked)
            update_current_appearance();

        is_blocked = false;
    }
}

void GateCell::reset()
{
    is_blocked = false;
    update_current_appearance();
}

void GateCell::update_current_appearance()
{
    if (is_blocked)
    {
        setPixmap(QPixmap(is_wall_above ? blocked_cell_ws : blocked_cell));
    }
    else
    {
        setPixmap(QPixmap(is_wall_above ? free_cell_ws : free_cell));
    }
}


