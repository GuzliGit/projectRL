#include "cellitem.h"
#include <QGraphicsScene>

CellItem::CellItem(QGraphicsItem *parent) :
    QGraphicsPixmapItem(parent),
    selection_animation(nullptr)
{
    QPixmap pixmap(":/img/img/EmptyCell.svg");
    width = pixmap.width();
    height = pixmap.height();

    setPixmap(pixmap);
    setTransformOriginPoint(this->boundingRect().center());

    setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable, false);
}

int CellItem::get_width()
{
    return width;
}

int CellItem::get_height()
{
    return height;
}

void CellItem::set_selected(bool cell_selected, bool ctrl_pressed)
{
    if (ctrl_pressed && cell_selected != selected && !current_selection_changed)
    {
        selected = !selected;
        current_selection_changed = true;
    }
    else if (!ctrl_pressed)
        selected = cell_selected;

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

bool CellItem::is_selected() const
{
    return selected;
}

void CellItem::update_cell_appearance()
{
    setPixmap(QPixmap(":/img/img/EmptyCell.svg"));
}

void CellItem::animate_selection()
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

        QPropertyAnimation *fade = new QPropertyAnimation(this, "opacity");
        fade->setDuration(1250);
        fade->setKeyValueAt(0, 1);
        fade->setKeyValueAt(0.25, 0.5);
        fade->setKeyValueAt(0.5, 0.3);
        fade->setKeyValueAt(0.75, 0.5);
        fade->setKeyValueAt(1, 1);
        fade->setLoopCount(-1);

        pulse->setEasingCurve(QEasingCurve::OutQuad);
        fade->setEasingCurve(QEasingCurve::InOutSine);

        selection_animation->addAnimation(pulse);
        selection_animation->addAnimation(fade);
    }

    if (selection_animation->state() != QAbstractAnimation::Running)
        selection_animation->start();
}

void CellItem::stop_animation()
{
    if (selection_animation)
    {
        selection_animation->stop();
        this->setScale(1.0);
        this->setOpacity(1.0);
    }
}

void CellItem::allow_selection()
{
    current_selection_changed = false;
}

bool CellItem::selection_was_changed() const
{
    return current_selection_changed;
}

void CellItem::reset_animation()
{
    stop_animation();
    animate_selection();
}
