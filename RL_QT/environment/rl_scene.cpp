#include "rl_scene.h"
#include "environment/cellitem.h"
#include "agent/agentobj.h"
#include "mainwindow.h"

#include <queue>
#include <QKeyEvent>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsView>
#include <QCursor>
#include <QScrollBar>
#include <QRectF>
#include <QMessageBox>
#include <QMainWindow>

RL_scene::RL_scene(int width, int height, int scale_factor, QObject* parent) :
    QGraphicsScene(parent)
{
    setSceneRect(0, 0, width * scale_factor, height * scale_factor);
    editor = new EnvironmentEditor();

    MainWindow *w = dynamic_cast<MainWindow*>(this->parent());
    if (w)
    {
        connect(w, &MainWindow::click_in_interactive_mode, this, &RL_scene::click_from_outside);
    }
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
    if (is_in_interactive_mode())
        return;

    for (auto cell : selected_cells)
    {
        for (auto agent : all_agents)
        {
            if (cell->pos() == agent->pos())
            {
                agent->remove_goal();
                all_agents.removeOne(agent);
                selected_agents.removeOne(agent);
                this->removeItem(agent);
            }
            else if (cell == agent->get_goal())
            {
                agent->remove_goal();
            }
        }
    }

    editor->change_cells(selected_cells, type);

    update_all_cells();
    deselect_cells();
}

void RL_scene::delete_selected_objs()
{
    if (is_in_interactive_mode())
        return;

    if (selected_cells.empty() && !selected_agents.empty())
    {
        for (auto agent : selected_agents)
        {
            all_agents.removeOne(agent);
            agent->remove_goal();
            this->removeItem(agent);
        }
        selected_agents.clear();
        this->update();
        return;
    }

    for (auto cell : selected_cells)
    {
        for (auto agent : all_agents)
        {
            if (cell->pos() == agent->pos())
            {
                agent->remove_goal();
                all_agents.removeOne(agent);
                selected_agents.removeOne(agent);
                this->removeItem(agent);
            }
            else if (cell == agent->get_goal())
            {
                agent->remove_goal();
            }
        }
    }

    editor->change_cells(selected_cells, CellType::Empty);

    update_all_cells();
    deselect_cells();
    this->update();
}

void RL_scene::add_agent(AgentType type)
{
    if ((selected_cells.size() == 0 && selected_agents.size() == 0) || is_in_interactive_mode())
        return;

    editor->add_agents(selected_cells, selected_agents, all_agents, type);

    deselect_cells();
    deselect_agents();
}

void RL_scene::set_agent_goal()
{
    if (selected_agents.size() == 0 || selected_agents.size() > 1)
        return;

    is_goal_selection = true;
    QCursor c;
    c.setShape(Qt::PointingHandCursor);

    QMainWindow *main_window = qobject_cast<QMainWindow*>(this->parent());
    main_window->setCursor(c);

    this->views().first()->setMouseTracking(true);
}

void RL_scene::remove_agent_goal()
{
    if (selected_agents.size() == 0 || selected_agents.size() > 1)
        return;

    selected_agents.first()->remove_goal();
}

bool RL_scene::is_in_interactive_mode()
{
    return is_goal_selection;
}

void RL_scene::load_cell(CellItem *cell)
{
    for (auto item : all_cells)
    {
        if (item->pos() == cell->pos())
        {
            all_cells.append(cell);
            all_cells.removeOne(item);
            this->removeItem(item);
            this->addItem(cell);
            return;
        }
    }
}

void RL_scene::load_agent(AgentObj *agent)
{
    all_agents.append(agent);
    this->addItem(agent);
}

void RL_scene::update_appearance()
{
    for (auto cell : all_cells)
    {
        cell->update_cell_appearance();
    }
}

bool RL_scene::is_correct_environment()
{
    if (this->width() <= 0 || all_cells.empty() || all_agents.empty())
        return false;

    for (auto agent : all_agents)
    {
        if (!agent->has_goal())
            return false;
    }

    if (!is_cells_connected())
        return false;

    return true;
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
    else if (event->button() == Qt::LeftButton && !is_goal_selection) // Выделение объектов на сцене
    {
        ctrl_pressed = event->modifiers() & Qt::ControlModifier;

        is_left_button_pressed = true;
        // Агенты
        AgentObj *temp = dynamic_cast<AgentObj*>(itemAt(event->scenePos(), QTransform()));
        CellItem *item = dynamic_cast<CellItem*>(itemAt(event->scenePos(), QTransform()));

        if (temp)
        {
            is_changing_agent_pos = false;
            deselect_cells();

            temp->set_selected(!temp->isSelected());

            for (auto agent : all_agents)
            {
                if (temp == agent)
                    selected_agents.append(agent);
                else
                {
                    agent->set_selected(false);
                    selected_agents.removeOne(agent);
                }
            }
        }

        // Клетки
        if (item)
        {
            deselect_agents();

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

        if (selected_agents.size() == 0)
        {
            selection_start = event->scenePos();
            selection_rect = new QGraphicsRectItem(QRectF(selection_start, QSizeF(0, 0)));
            selection_rect->setPen(QPen(Qt::white, 1, Qt::DashLine));
            addItem(selection_rect);
        }

        event->accept();
    }
    else if (event->button() == Qt::LeftButton && is_goal_selection)
    {
        CellItem *item = dynamic_cast<CellItem*>(itemAt(event->scenePos(), QTransform()));
        bool is_already_goal = false;

        deselect_cells();
        if (item)
        {
            if (item->is_walkable() && selected_agents.size() == 1)
            {
                for (auto agent : all_agents)
                {
                    if (agent == selected_agents.first())
                        continue;

                    if (item == agent->get_goal())
                        is_already_goal = true;
                }

                if (!is_already_goal)
                    selected_agents.first()->set_goal(item);
            }
        }

        event->accept();
    }
    else if (event->button() == Qt::RightButton && !selection_rect)
    {
        deselect_cells();
        deselect_agents();

        if (is_goal_selection)
        {
            QCursor c;
            c.setShape(Qt::ArrowCursor);

            QMainWindow *main_window = qobject_cast<QMainWindow*>(this->parent());
            main_window->setCursor(c);

            is_goal_selection = false;
            emit update_settings();
        }

        event->accept();
    }
    else
    {
        QGraphicsScene::mousePressEvent(event);
    }
    this->update();
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
    else if (selected_agents.size() == 1 && is_left_button_pressed)
    {
        CellItem *item = dynamic_cast<CellItem*>(itemAt(event->scenePos(), QTransform()));
        bool is_already_goal = false;

        for (auto agent : all_agents)
        {
            if (agent->has_goal() && agent->get_goal() == item)
            {
                is_already_goal = true;
                break;
            }
        }

        if (item && item->is_walkable() && !is_already_goal)
        {
            is_changing_agent_pos = true;
            selected_agents.first()->setPos(item->pos());
        }

        event->accept();
    }
    else if (is_goal_selection)
    {
        CellItem *item = dynamic_cast<CellItem*>(itemAt(event->scenePos(), QTransform()));

        if (item && !selected_cells.contains(item))
        {
            deselect_cells();
            item->set_selected(true, false);
            selected_cells.append(item);
        }

        event->accept();
    }
    else
    {
        QGraphicsScene::mouseMoveEvent(event);
    }
    this->update();
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

        is_left_button_pressed = false;

        event->accept();
    }
    else if (is_changing_agent_pos)
    {
        deselect_agents();
        is_left_button_pressed = false;

        event->accept();
    }
    else if (is_goal_selection)
    {
        is_goal_selection = false;
        QCursor c;
        c.setShape(Qt::ArrowCursor);

        QMainWindow *main_window = qobject_cast<QMainWindow*>(this->parent());
        main_window->setCursor(c);

        this->views().first()->setMouseTracking(true);
        event->accept();
    }
    else
    {
        is_left_button_pressed = false;
        QGraphicsScene::mouseReleaseEvent(event);
    }
    this->update();
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

void RL_scene::click_from_outside()
{
    if (is_goal_selection)
    {
        deselect_cells();
        is_goal_selection = false;
        emit update_settings();
    }
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

void RL_scene::deselect_agents()
{
    for (auto agent : selected_agents)
    {
        agent->set_selected(false);
    }
    selected_agents.clear();
}

bool RL_scene::is_cells_connected()
{
    CellItem *temp = new CellItem;
    int width_scale = temp->get_width();
    int height_scale = temp->get_height();
    int rows = this->width() / width_scale;
    int cols = this->height() / height_scale;
    bool is_free_cells = false;

    QPointF search_start = all_agents.first()->pos();

    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
    std::queue<QPointF> q;
    q.push(search_start);
    visited[search_start.x() / width_scale][search_start.y() / height_scale] = true;

    const std::vector<QPointF> directions = { QPointF(-width_scale, 0),
                                             QPointF(width_scale, 0),
                                             QPointF(0, height_scale),
                                             QPointF(0, -height_scale)};

    while (!q.empty())
    {
        QPointF point = q.front();
        q.pop();

        for (const auto& next : directions)
        {
            int nx = point.x() + next.x();
            int ny = point.y() + next.y();

            if (nx >= 0 && nx / width_scale < rows && ny >= 0 && ny / height_scale < cols)
            {
                if (!visited[nx / width_scale][ny / height_scale])
                {
                    CellItem *cell = dynamic_cast<CellItem*>(this->items(QPointF(nx, ny)).first());
                    AgentObj *agent = dynamic_cast<AgentObj*>(this->items(QPointF(nx, ny)).first());

                    if ((cell && cell->is_walkable()) || agent)
                    {
                        is_free_cells = cell ? true : is_free_cells;
                        visited[nx / width_scale][ny / height_scale] = true;
                        q.push(QPointF(nx, ny));
                    }
                }
            }
        }
    }

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            CellItem *cell = dynamic_cast<CellItem*>(this->items(QPointF(i * width_scale, j * height_scale)).first());
            AgentObj *agent = dynamic_cast<AgentObj*>(this->items(QPointF(i * width_scale, j * height_scale)).first());

            if ((cell && cell->is_walkable() && !visited[i][j]) || (agent && !visited[i][j]))
            {
                return false;
            }
        }
    }

    return true;
}
