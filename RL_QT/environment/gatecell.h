#ifndef GATECELL_H
#define GATECELL_H

#include "cellitem.h"

#include <random>
#include <QObject>

class GateCell : public CellItem
{
    Q_OBJECT
public:
    GateCell(QGraphicsItem *parent = nullptr);
    virtual CellType get_type() const override { return CellType::Gate; }
    virtual void update_cell_appearance() override;
    virtual bool is_walkable() const override { return !is_blocked; }
    int get_closing_chance();
    void set_closing_chance(int val);
    virtual void update_status(bool has_agent, bool visualize) override;
    virtual void reset() override;

private:
    bool is_blocked = false;
    bool is_wall_above = false;
    int closing_chance = 50;
    std::mt19937 rng;
    std::uniform_int_distribution<int> dist;
    QPixmap free_cell, free_cell_ws, blocked_cell, blocked_cell_ws;

    void update_current_appearance();
};

#endif // GATECELL_H
