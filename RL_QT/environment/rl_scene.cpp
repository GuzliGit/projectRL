#include "rl_scene.h"
#include "environment/cellitem.h"
#include <QKeyEvent>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsView>
#include <QCursor>
#include <QScrollBar>
#include <QRectF>

RL_scene::RL_scene(int width, int height, int scale_factor, QObject* parent) :
    QGraphicsScene(parent)
{
    setSceneRect(0, 0, width * scale_factor, height * scale_factor);
    editor = new EnvironmentEditor();
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
            all_cells.append(cell);
        }
    }
}

void RL_scene::change_selected_cells(CellType type)
{
    editor->change_cells(selected_cells, type);

    update_all_cells();

    deselect_cells();
}

void RL_scene::delete_selected_objs()
{
    // Добавить удаление агента позже...

    if (selected_cells.empty())
        return;

    editor->change_cells(selected_cells, CellType::Empty);

    update_all_cells();
    deselect_cells();
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
    if (event->button() == Qt::MiddleButton) // Навигация в среде с помощью СКМ
    {
        is_panning = true;
        QCursor c;
        c.setShape(Qt::ClosedHandCursor);
        this->views().first()->setCursor(c);

        event->accept();
    }
    else if (event->button() == Qt::LeftButton) // Выделение клеток
    {
        ctrl_pressed = event->modifiers() & Qt::ControlModifier;
        CellItem *item = dynamic_cast<CellItem*>(itemAt(event->scenePos(), QTransform()));

        if (item)
        {
            if (!ctrl_pressed)
            {
                selected_cells.clear();

                for (auto cell : all_cells)
                {
                    if (cell == item)
                    {
                        cell->set_selected(!cell->is_selected(), false);
                        if (cell->is_selected())
                            selected_cells.append(cell);
                    }
                    else
                        cell->set_selected(false, false);
                }
            }
            else
            {
                item->set_selected(!item->is_selected(), ctrl_pressed);

                if (selected_cells.contains(item) && !item->is_selected())
                    selected_cells.removeOne(item);
                else if (!selected_cells.contains(item) && item->is_selected())
                    selected_cells.append(item);
            }
        }

        selection_start = event->scenePos();
        selection_rect = new QGraphicsRectItem(QRectF(selection_start, QSizeF(0, 0)));
        selection_rect->setPen(QPen(Qt::white, 1, Qt::DashLine));
        addItem(selection_rect);

        event->accept();
    }
    else if (event->button() == Qt::RightButton && !selection_rect)
    {
        deselect_cells();

        event->accept();
    }
    else
    {
        QGraphicsScene::mousePressEvent(event);
    }
}

void RL_scene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (is_panning) // Навигация в среде с помощью СКМ
    {
        QList<QGraphicsView*> views = this->views();
        QGraphicsView *view = views.first();

        QPointF delta = event->screenPos() - event->lastScreenPos();

        view->horizontalScrollBar()->setValue(view->horizontalScrollBar()->value() - delta.x());
        view->verticalScrollBar()->setValue(view->verticalScrollBar()->value() - delta.y());

        event->accept();
    }
    else if (selection_rect) // Выделение клеток
    {
        QRectF rect(selection_start, event->scenePos());
        selection_rect->setRect(rect.normalized());

        if (!ctrl_pressed)
        {
            for (auto cell : selected_cells)
                cell->set_selected(false, ctrl_pressed);

            selected_cells.clear();
        }

        for (auto cell : all_cells)
        {
            bool is_intersects = rect.intersects(cell->sceneBoundingRect());
            if (is_intersects)
            {
                cell->set_selected(!cell->is_selected(), ctrl_pressed);

                if (selected_cells.contains(cell) && !cell->is_selected())
                    selected_cells.removeOne(cell);
                else if (!selected_cells.contains(cell) && cell->is_selected())
                    selected_cells.append(cell);
            }
            else if (cell->selection_was_changed() && !is_intersects)
            {
                cell->allow_selection();
                cell->set_selected(!cell->is_selected(), ctrl_pressed);
                if (selected_cells.contains(cell) && !cell->is_selected())
                    selected_cells.removeOne(cell);
                else if (!selected_cells.contains(cell) && cell->is_selected())
                    selected_cells.append(cell);

                cell->allow_selection();
            }
        }

        event->accept();
    }
    else
    {
        QGraphicsScene::mouseMoveEvent(event);
    }
}

void RL_scene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) // Навигация в среде с помощью СКМ
    {
        is_panning = false;
        QCursor c;
        c.setShape(Qt::ArrowCursor);
        this->views().first()->setCursor(c);

        event->accept();
    }
    else if (selection_rect && Qt::LeftButton) // Выделение клеток
    {
        removeItem(selection_rect);
        delete selection_rect;
        selection_rect = nullptr;

        for (auto cell : all_cells)
        {
            cell->allow_selection();
        }

        synchronize_animation();

        // // Проверка выбора
        // qDebug() << "New Selection\n";
        // for (auto cell : selected_cells)
        // {
        //     qDebug() << cell->x() << cell->y();
        // }

        // event->accept();
    }
    else
    {
        QGraphicsScene::mouseReleaseEvent(event);
    }
}

void RL_scene::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Control)
    {
        ctrl_pressed = true;
    }
    QGraphicsScene::keyPressEvent(event);
}

void RL_scene::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Control)
    {
        ctrl_pressed = false;
    }
    QGraphicsScene::keyReleaseEvent(event);
}

void RL_scene::synchronize_animation()
{
    for (auto cell : selected_cells)
        cell->reset_animation();
}

void RL_scene::update_all_cells()
{
    for (auto cell : selected_cells)
    {
        QPointF cell_pos = cell->pos();

        for (int i = 0; i < all_cells.size(); i++)
        {
            if (all_cells[i]->pos() == cell_pos)
            {
                all_cells[i] = cell;
                break;
            }
        }
    }
}

void RL_scene::deselect_cells()
{
    for (auto cell : selected_cells)
    {
        cell->set_selected(false, false);
    }
    selected_cells.clear();
}
