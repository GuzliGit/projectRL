#include "rl_scene.h"
#include "cellitem.h"
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsView>
#include <QCursor>
#include <QScrollBar>

RL_scene::RL_scene(int width, int height, int scale_factor, QObject* parent) : QGraphicsScene(parent)
{
    setSceneRect(0, 0, width * scale_factor, height * scale_factor);
}

void RL_scene::fill_with_empty_cells()
{
    CellItem *temp = new CellItem();
    int cell_width = temp->get_width();
    int cell_height = temp->get_height();
    int cols = this->width() / cell_width;
    int rows = this->height() / cell_height;

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            CellItem *cell = new CellItem();
            cell->setPos(j * cell_width, i * cell_height);
            this->addItem(cell);
        }
    }
}

void RL_scene::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    QList<QGraphicsView*> views = this->views();

    QGraphicsView *view = views.first();
    int delta = event->delta();

    if (delta > 0)
        current_scale += scale_step;
    else
        current_scale = qMax(0.5, current_scale - scale_step);

    view->setTransform(QTransform::fromScale(current_scale, current_scale));

    event->accept();
}

void RL_scene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton)
    {
        is_panning = true;
        QCursor c;
        c.setShape(Qt::ClosedHandCursor);
        this->views().first()->setCursor(c);

        event->accept();
    }
    else
    {
        QGraphicsScene::mousePressEvent(event);
    }
}

void RL_scene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (is_panning)
    {
        QList<QGraphicsView*> views = this->views();
        QGraphicsView *view = views.first();

        QPointF delta = event->screenPos() - event->lastScreenPos();

        view->horizontalScrollBar()->setValue(view->horizontalScrollBar()->value() - delta.x());
        view->verticalScrollBar()->setValue(view->verticalScrollBar()->value() - delta.y());

        event->accept();
    }
    else
    {
        QGraphicsScene::mouseMoveEvent(event);
    }
}

void RL_scene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton)
    {
        is_panning = false;
        QCursor c;
        c.setShape(Qt::ArrowCursor);
        this->views().first()->setCursor(c);

        event->accept();
    } else
    {
        QGraphicsScene::mouseReleaseEvent(event);
    }
}
