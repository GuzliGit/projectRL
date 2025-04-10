#include "agentobj.h"

#include <QGraphicsItem>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QGraphicsSceneMouseEvent>
#include <QPen>

AgentObj::AgentObj(QGraphicsItem *parent) :
    QGraphicsPixmapItem(parent),
    selection_animation(nullptr)
{
    QPixmap pixmap(":/img/img/Agent.svg");
    width = pixmap.width();
    height = pixmap.height();

    view_cirlce = new QGraphicsEllipseItem(-view_range, -view_range, view_range * 2, view_range * 2, this);
    QPen pen(QColor::fromRgb(1, 1, 1));
    pen.setWidth(2);
    view_cirlce->setPen(pen);
    view_cirlce->setOpacity(0);
    view_cirlce->setPos(pos().x() + width / 2, pos().y() + height / 2);

    setPixmap(pixmap);
    setTransformOriginPoint(this->boundingRect().center());

    setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable, false);
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
        view_cirlce->setOpacity(1);
    }
    else
    {
        stop_animation();
        view_cirlce->setOpacity(0);
    }

    this->setSelected(selected);
}

bool AgentObj::is_selected() const
{
    return selected;
}

void AgentObj::set_view_range(int range)
{
    view_range = range;

    view_cirlce = new QGraphicsEllipseItem(-view_range, -view_range, view_range * 2, view_range * 2, this);
    QPen pen(QColor::fromRgb(1, 1, 1));
    pen.setWidth(2);
    view_cirlce->setPen(pen);
    view_cirlce->setOpacity(0);
    view_cirlce->setPos(pos().x() + width / 2, pos().y() + height / 2);
}

void AgentObj::animate_selection()
{
    if (!selection_animation)
    {
        selection_animation = new QParallelAnimationGroup(this);

        QPropertyAnimation *pulse = new QPropertyAnimation(this, "scale");
        pulse->setDuration(1250);
        pulse->setKeyValueAt(0, 1.0);
        pulse->setKeyValueAt(0.5, 0.65);
        pulse->setKeyValueAt(1, 1);
        pulse->setLoopCount(-1);

        pulse->setEasingCurve(QEasingCurve::OutQuad);

        selection_animation->addAnimation(pulse);
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
        view_cirlce->setOpacity(0);
    }
}
