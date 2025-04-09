#include "agentobj.h"

#include <QGraphicsItem>
#include <QParallelAnimationGroup>

AgentObj::AgentObj(QGraphicsItem *parent) :
    QGraphicsPixmapItem(parent),
    selection_animation(nullptr)
{
    QPixmap pixmap(":/img/img/Agent.svg");
    width = pixmap.width();
    height = pixmap.height();

    setPixmap(pixmap);

    setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable, true);
}

int AgentObj::get_width()
{
    return width;
}

int AgentObj::get_height()
{
    return height;
}

void AgentObj::set_selected(bool agent_selected)
{
    selected = agent_selected;

    if (selected)
    {
        animate_selection();
    }
    else
    {
        stop_animation();
    }

    this->setSelected(selected);
}

bool AgentObj::is_selected() const
{
    return selected;
}

void AgentObj::animate_selection()
{
    if (!selection_animation)
    {

    }
}

void AgentObj::stop_animation()
{

}
