#include "agentobj.h"

#include <QGraphicsItem>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QGraphicsSceneMouseEvent>
#include <QPen>
#include <QPainter>
#include <QGraphicsView>
#include <QFileDialog>

AgentObj::AgentObj(QGraphicsItem *parent) :
    QGraphicsPixmapItem(parent),
    selection_animation(nullptr)
{
    QPixmap pixmap(":/img/img/Agent.svg");
    width = pixmap.width();
    height = pixmap.height();
    setZValue(1);

    goal_pixmap = new QGraphicsPixmapItem;
    goal_pixmap->setPixmap(QPixmap(":/img/img/Goal.svg"));
    goal_pixmap->setFlag(QGraphicsItem::ItemIsSelectable, false);
    goal_pixmap->setFlag(QGraphicsItem::ItemIsMovable, false);

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

void AgentObj::set_goal(CellItem *new_goal)
{
    if (!new_goal->is_walkable())
        return;

    goal_cell = new_goal;
    goal_pixmap->setPos(goal_cell->pos());

    if (this->scene()->items().contains(goal_pixmap))
        return;

    this->scene()->addItem(goal_pixmap);
}

CellItem *AgentObj::get_goal()
{
    if (goal_cell)
        return goal_cell;

    return nullptr;
}

bool AgentObj::has_goal()
{
    if (goal_cell)
        return true;

    return false;
}

void AgentObj::remove_goal()
{
    if (!goal_cell)
        return;

    goal_cell = nullptr;
    this->scene()->removeItem(goal_pixmap);
}

short AgentObj::get_current_state()
{
    int row_pos = this->pos().x() / width;
    int cols_below = this->pos().y() / height;
    int rows_shift = cols_below * this->scene()->width() / width;
    return (rows_shift + row_pos);
}

void AgentObj::save_start_point()
{
    start_pos = this->pos();
}

void AgentObj::reset_pos()
{
    if (start_pos.x() >= 0)
        this->setPos(start_pos);
}

bool AgentObj::is_done()
{
    return done;
}

void AgentObj::set_done(bool done_status)
{
    done = done_status;
}

bool AgentObj::has_learning_file()
{
    return learn_path != nullptr;
}

QString AgentObj::get_learning_file_path()
{
    if (learn_path != nullptr)
        return learn_path;
    else
        return "Не задан";
}

void AgentObj::set_learning_file(QString file_path)
{
    learn_path = file_path;
}

double **AgentObj::get_q_table()
{
    return Q;
}

void AgentObj::set_q_table(double **q, int states, int actions)
{
    delete_learning_file();
    state_size = states;

    Q = new double*[state_size];
    for (int i = 0; i < state_size; i++)
    {
        Q[i] = new double[actions];
        for (int j = 0; j < actions; j++)
        {
            Q[i][j] = q[i][j];
        }
    }
}

void AgentObj::delete_learning_file()
{
    if (Q != nullptr) {
        for (int i = 0; i < state_size; i++)
            delete[] Q[i];

        delete[] Q;
    }

    Q = nullptr;
    learn_path = nullptr;
}

void AgentObj::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsPixmapItem::paint(painter, option, widget);

    if (selected) {

        if (goal_cell)
        {
            qreal label_size = qMin(boundingRect().width(), boundingRect().height()) * 0.3;
            QRectF label_rect(boundingRect().width() - label_size, boundingRect().height() - label_size, label_size, label_size);

            painter->setPen(Qt::black);
            painter->setBrush(Qt::green);
            painter->drawEllipse(label_rect);

            QRectF temp_rect = goal_cell->boundingRect();
            QPointF goal_pos = mapFromScene(goal_cell->scenePos());
            QRectF goal_rect(goal_pos.x() + temp_rect.width() - label_size, goal_pos.y() + temp_rect.height() - label_size, label_size, label_size);

            painter->setPen(Qt::black);
            painter->setBrush(Qt::green);
            painter->setOpacity(1);
            painter->drawEllipse(goal_rect);
        }
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
