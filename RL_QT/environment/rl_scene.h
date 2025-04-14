#ifndef RL_SCENE_H
#define RL_SCENE_H

#include "environment/cellitem.h"
#include "environment/environmenteditor.h"
#include "agent/agentobj.h"

#include <QGraphicsScene>
#include <QPointF>

class RL_scene : public QGraphicsScene
{
    Q_OBJECT
public:
    RL_scene(int width, int height, int scale_factor, QObject* parent = nullptr);
    void fill_with_empty_cells();
    void change_selected_cells(CellType type);
    void delete_selected_objs();
    void add_agent(AgentType type);
    void set_agent_goal();
    void remove_agent_goal();
    bool is_in_interactive_mode();

protected:
    void wheelEvent(QGraphicsSceneWheelEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    EnvironmentEditor *editor;

    double current_scale = 1.0;
    const double scale_step = 0.1;
    bool is_panning = false;
    bool is_changing_agent_pos = false;
    bool is_left_button_pressed = false;
    bool is_goal_selection = false;

    QPointF selection_start;
    bool ctrl_pressed = false;
    QGraphicsRectItem *selection_rect = nullptr;
    QList<CellItem*> all_cells;
    QList<AgentObj*> all_agents;
    QList<CellItem*> selected_cells;
    QList<AgentObj*> selected_agents;

    void synchronize_animation();
    void update_all_cells();
    void deselect_cells();
    void deselect_agents();
};

#endif // RL_SCENE_H
