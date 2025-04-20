#ifndef AGENTOBJ_H
#define AGENTOBJ_H

#include "environment/cellitem.h"

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QParallelAnimationGroup>
#include <QGraphicsSceneMouseEvent>

//#define MIN_VIEW_RANGE 32

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
    //int get_view_range();
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
    //virtual void set_view_range(int range);
    virtual AgentType get_type() { return AgentType::Default; }

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
    int width;
    int height;
    //int view_range;
    bool selected = false;
    bool done = false;
    QPointF start_pos = QPointF(-10, -10);
    QParallelAnimationGroup *selection_animation;
    QPainter* painter;
    CellItem* goal_cell = nullptr;
    QGraphicsPixmapItem* goal_pixmap = nullptr;

    void animate_selection();
    void stop_animation();
};

#endif // AGENTOBJ_H
