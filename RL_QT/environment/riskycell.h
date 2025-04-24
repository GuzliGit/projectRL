#ifndef RISKYCELL_H
#define RISKYCELL_H

#include "cellitem.h"

#include <random>
#include <QObject>

class RiskyCell : public CellItem
{
    Q_OBJECT
public:
    RiskyCell(QGraphicsItem *parent = nullptr);
    virtual CellType get_type() const override { return CellType::Risky; }
    virtual void update_cell_appearance() override;
    virtual bool is_walkable() const override { return !is_stucked; }
    int get_stuck_chance();
    void set_stuck_chance(int val);
    virtual void update_status(bool has_agent) override;
    virtual void reset() override;

private:
    bool is_stucked = false;
    bool is_wall_above = false;
    int stuck_chance = 50;
    std::mt19937 rng;
    std::uniform_int_distribution<int> dist;

    QPixmap free_cell, free_cell_ws, stucked_cell, stucked_cell_ws;
    void update_current_appearance();
};

#endif // RISKYCELL_H
