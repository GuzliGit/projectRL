#ifndef AGENTOBJ_H
#define AGENTOBJ_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QParallelAnimationGroup>
#include <QGraphicsSceneMouseEvent>

enum AgentType
{
    LimitedView
};

class AgentObj : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_PROPERTY(qreal scale READ scale WRITE setScale)        // Масштаб
public:
    AgentObj(QGraphicsItem *parent = nullptr);
    int get_width();
    int get_height();
    void set_selected(bool agent_selected);
    bool is_selected() const;
    void set_view_range(int range);
    virtual AgentType get_type() { return AgentType::LimitedView; }

private:
    int width;
    int height;
    int view_range = 64;
    bool selected = false;
    QParallelAnimationGroup *selection_animation;
    QGraphicsEllipseItem *view_cirlce;

    void animate_selection();
    void stop_animation();
};

#endif // AGENTOBJ_H
