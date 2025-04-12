#include "agentobj.h"

#include <QGraphicsItem>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QGraphicsSceneMouseEvent>
#include <QPen>
#include <QPainter>
#include <QGraphicsView>

AgentObj::AgentObj(QGraphicsItem *parent) :
    QGraphicsPixmapItem(parent),
    selection_animation(nullptr)
{
    QPixmap pixmap(":/img/img/Agent.svg");
    width = pixmap.width();
    height = pixmap.height();
    view_range = MIN_VIEW_RANGE;

    setPixmap(pixmap);
    setTransformOriginPoint(this->boundingRect().center());

    setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable, false);
}

AgentObj::~AgentObj()
{
    stop_animation();
    this->set_selected(false);
    disconnect();
}

int AgentObj::get_width()
{
    return width;
}

int AgentObj::get_height()
{
    return height;
}

int AgentObj::get_view_range()
{
    return view_range / width;
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

void AgentObj::set_view_range(int range)
{
    if (range < MIN_VIEW_RANGE)
        return;

    view_range = range;
    this->scene()->update();
}

void AgentObj::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsPixmapItem::paint(painter, option, widget);

    if (selected) {
        painter->save();

        painter->setPen(QPen(Qt::black, 2));
        painter->setOpacity(0.8);
        painter->setBrush(Qt::NoBrush);

        painter->drawEllipse(boundingRect().center(), view_range, view_range);

        painter->restore();
    }
}

void AgentObj::animate_selection()
{
    if (!selection_animation)
    {
        selection_animation = new QParallelAnimationGroup(this);

        QPropertyAnimation *fade = new QPropertyAnimation(this, "opacity");
        fade->setDuration(1250);
        fade->setKeyValueAt(0, 1);
        fade->setKeyValueAt(0.25, 0.5);
        fade->setKeyValueAt(0.5, 0.3);
        fade->setKeyValueAt(0.75, 0.5);
        fade->setKeyValueAt(1, 1);
        fade->setLoopCount(-1);

        fade->setEasingCurve(QEasingCurve::InOutSine);

        selection_animation->addAnimation(fade);
    }

    if (selection_animation->state() != QAbstractAnimation::Running)
        selection_animation->start();
}

void AgentObj::stop_animation()
{
    if (selection_animation)
    {
        selection_animation->stop();
        this->setScale(1.0);
        this->setOpacity(1.0);
    }
}
