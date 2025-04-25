#ifndef AGENTOBJ_H
#define AGENTOBJ_H

#include "environment/cellitem.h"

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QParallelAnimationGroup>
#include <QGraphicsSceneMouseEvent>

enum AgentType
{
    Default
};

enum AgentActions
{
    MoveForward,
    MoveBackwards,
    MoveToTheLeft,
    MoveToTheRight
};

class AgentObj : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)  // Прозрачность
public:
    AgentObj(QGraphicsItem *parent = nullptr);
    ~AgentObj();
    int get_width();
    int get_height();
    void set_selected(bool agent_selected);
    bool is_selected() const;
    void set_goal(CellItem* new_goal);
    CellItem *get_goal();
    bool has_goal();
    void remove_goal();
    short get_current_state();
    void save_start_point();
    void reset_pos();
    bool is_done();
    void set_done(bool done_status);
    virtual AgentType get_type() { return AgentType::Default; }
    bool has_learning_file();
    QString get_learning_file_path();
    void set_learning_file(QString file_path);
    double **get_q_table();
    void set_q_table(double **q, int states, int actions = 4);
    void delete_learning_file();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
    int width;
    int height;
    bool selected = false;
    bool done = false;
    QPointF start_pos = QPointF(-10, -10);
    QParallelAnimationGroup *selection_animation;
    QPainter* painter;
    CellItem* goal_cell = nullptr;
    QGraphicsPixmapItem* goal_pixmap = nullptr;

    QString learn_path = nullptr;
    double **Q = nullptr;
    int state_size;

    void animate_selection();
    void stop_animation();
};

#endif // AGENTOBJ_H
