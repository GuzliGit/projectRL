#ifndef RL_SCENE_H
#define RL_SCENE_H

#define WALL_REWARD -5
#define AGENT_COLLISION_REWARD -3
#define FLOOR_REWARD -1
#define GOAL_REWARD 120

#include "environment/cellitem.h"
#include "environment/environmenteditor.h"
#include "agent/agentobj.h"

#include <QGraphicsScene>
#include <QPointF>

enum TrainAlgorithms
{
    QLearn
};

class RL_scene : public QGraphicsScene
{
    Q_OBJECT
    friend class QLearningTrainer;
public:
    RL_scene(int width, int height, int scale_factor, QObject* parent = nullptr);
    void clear_selection();
    void fill_with_empty_cells();
    void change_selected_cells(CellType type);
    void delete_selected_objs();
    void add_agent(AgentType type);
    void set_agent_goal();
    void remove_agent_goal();
    bool is_in_interactive_mode();
    bool is_visualize_status();
    void set_visualize_status(bool val);
    bool is_training();
    void set_training(bool val);
    void load_cell(CellItem* cell);
    void load_agent(AgentObj* agent);
    void update_appearance();
    bool is_correct_environment();
    void start_qlearn(double alpha_t, double gamma_t, double epsilon_t, int episodes_count);

protected:
    void wheelEvent(QGraphicsSceneWheelEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

public slots:
    signed char execute_action_in_main(int agent_id, QPointF new_pos)
    {
        return execute_action(agent_id, new_pos);
    };
    bool get_agents_done_status_in_main(int agent_id);
    void reset_env();
    void prepare_for_learning();

private slots:
    void click_from_outside();

signals:
    void update_settings();
    void update_logs(QVector<QVector<int>> rewards);
    void learning_finished(QVector<QVector<int>> rewards, QPointF *coords);

private:
    EnvironmentEditor *editor;

    double current_scale = 1.0;
    const double scale_step = 0.1;
    bool is_panning = false;
    bool is_changing_agent_pos = false;
    bool is_left_button_pressed = false;
    bool is_goal_selection = false;
    bool is_visualize_mod = false;
    bool is_training_mod = false;

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
    bool is_cells_connected(QList<QPointF>& blocked_targets);
    bool is_all_goals_reachable();
    void get_agents_states(short *states);
    void set_actions_get_rewards(char *actions, signed char *rewards);
    bool is_new_point_inside_scene(QPointF new_point);
    signed char execute_action(int agent_id, QPointF new_pos);
    void get_agents_done_status(char *dones);
};

#endif // RL_SCENE_H
