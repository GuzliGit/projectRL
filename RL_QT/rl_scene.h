#ifndef RL_SCENE_H
#define RL_SCENE_H

#include "cellitem.h"
#include <QGraphicsScene>
#include <QPointF>

class RL_scene : public QGraphicsScene
{
public:
    RL_scene(int width, int height, int scale_factor, QObject* parent = nullptr);
    void fill_with_empty_cells();

protected:
    void wheelEvent(QGraphicsSceneWheelEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    double current_scale = 1.0;
    const double scale_step = 0.1;
    bool is_panning = false;

    QPointF selection_start;
    bool ctrl_pressed = false;
    QGraphicsRectItem *selection_rect = nullptr;
    QList<CellItem*> all_cells;
    QList<CellItem*> selected_cells;

    void synchronize_animation();
};

#endif // RL_SCENE_H
