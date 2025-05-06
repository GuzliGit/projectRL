#include "riskycell.h"
#include "wallcell.h"

#include <random>
#include <QGraphicsScene>

RiskyCell::RiskyCell(QGraphicsItem *parent) : CellItem(parent)
{
    free_cell = QPixmap(":/img/img/RiskyCellFree.svg");
    free_cell_ws = QPixmap(":/img/img/RiskyCellFreeWithShadow.svg");
    stucked_cell = QPixmap(":/img/img/RiskyCellStucked.svg");
    stucked_cell_ws = QPixmap(":/img/img/RiskyCellStuckedWithShadow.svg");

    setPixmap(free_cell);

    std::mt19937 rng_t(std::random_device{}());
    this->rng = rng_t;
    std::uniform_int_distribution<int> dist_t(0, 99);
    this->dist = dist_t;
}

void RiskyCell::update_cell_appearance()
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

    if (is_stucked)
    {
        setPixmap(QPixmap(is_wall_above ? stucked_cell_ws : stucked_cell));
    }
    else
    {
        setPixmap(QPixmap(is_wall_above ? free_cell_ws : free_cell));
    }
}

int RiskyCell::get_stuck_chance()
{
    return stuck_chance;
}

void RiskyCell::set_stuck_chance(int val)
{
    stuck_chance = val;
}

void RiskyCell::update_status(bool has_agent)
{
    bool old_status = is_stucked;

    if (has_agent)
    {
        int random_val = dist(rng);
        is_stucked = (random_val < stuck_chance);
    }
    else
    {
        is_stucked = false;
    }

    if (old_status != is_stucked)
        update_current_appearance();
}

void RiskyCell::reset()
{
    is_stucked = false;
    update_current_appearance();
}

void RiskyCell::update_current_appearance()
{
    if (is_stucked)
    {
        setPixmap(QPixmap(is_wall_above ? stucked_cell_ws : stucked_cell));
    }
    else
    {
        setPixmap(QPixmap(is_wall_above ? free_cell_ws : free_cell));
    }
}
