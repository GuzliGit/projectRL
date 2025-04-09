#ifndef AGENTOBJ_H
#define AGENTOBJ_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QParallelAnimationGroup>

class AgentObj : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    AgentObj(QGraphicsItem *parent = nullptr);
    int get_width();
    int get_height();
    void set_selected(bool agent_selected);
    bool is_selected() const;

private:
    int width;
    int height;
    bool selected = false;
    QParallelAnimationGroup *selection_animation;

    void animate_selection();
    void stop_animation();
};

#endif // AGENTOBJ_H
