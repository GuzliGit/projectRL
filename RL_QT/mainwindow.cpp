#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "createproj_dialog.h"
#include "custom_tools/widgetwithflowlayout.h"
#include "agent/agentobj.h"
#include "environment/cellitem.h"
#include "environment/floorcell.h"
#include "environment/wallcell.h"

#include <QDockWidget>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QTabWidget>
#include <QButtonGroup>
#include <QDialog>
#include <QMessageBox>
#include <QDebug>
#include <QPainter>
#include <QScrollArea>
#include <QSettings>
#include <QSpinBox>
#include <QFileDialog>
#include <QComboBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , scene(new RL_scene(0, 0, SCALE_FACTOR, this))
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setup_toolbar();
    setup_widgets();
    setup_editor_panel_widgets();
    setup_settings_panel_widgets();

    QSettings settings("badin_AP-126", "RL_studio");
    restoreState(settings.value("window_state").toByteArray());

    connect(scene, &RL_scene::selectionChanged, this, &MainWindow::onScene_selection_changed);
    connect(scene, &RL_scene::update_settings, this, &MainWindow::onScene_selection_changed);
}

MainWindow::~MainWindow()
{
    delete scene;
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (this->scene && this->scene->is_in_interactive_mode())
    {
        QCursor c;
        c.setShape(Qt::ArrowCursor);
        this->setCursor(c);

        emit click_in_interactive_mode();

        event->accept();
    }
    else
    {
        QMainWindow::mousePressEvent(event);
    }
}

void MainWindow::setup_widgets()
{
    QWidget *central_widget = new QWidget(this);
    setCentralWidget(central_widget);

    editor_dock = new QDockWidget("Редактор", this);
    editor_dock->setObjectName("editor");

    log_dock = new QDockWidget("Логи", this);
    log_dock->setObjectName("logs");

    settings_dock = new QDockWidget("Настройки", this);
    settings_dock->setObjectName("settings");

    editor_dock->setWidget(ui->editor_panel);
    log_dock->setWidget(ui->log_panel);
    settings_dock->setWidget(ui->rl_settings_panel);

    addDockWidget(Qt::LeftDockWidgetArea, editor_dock);
    addDockWidget(Qt::BottomDockWidgetArea, log_dock);
    addDockWidget(Qt::RightDockWidgetArea, settings_dock);

    editor_dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetFloatable);
    log_dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetFloatable);
    settings_dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetFloatable);

    setCentralWidget(ui->environment);
}

QPushButton * MainWindow::create_editor_panel_button(QString btn_name, int btn_w, int btn_h, QString icon_path, QSize icon_size, QWidget *parent)
{
    QPushButton *btn = new QPushButton(parent);
    btn->setFixedSize(btn_w, btn_h);
    btn->setIcon(QIcon(icon_path));
    btn->setIconSize(icon_size);
    btn->setLayout(new QVBoxLayout);

    QLabel *temp = new QLabel(btn_name);
    temp->setAlignment(Qt::AlignCenter | Qt::AlignBottom);
    btn->layout()->addWidget(temp);

    return btn;
}

void MainWindow::setup_editor_panel_widgets()
{
    QTabWidget *tabs = new QTabWidget;

    // Раздел с поверхностями
    WidgetWithFlowLayout *floor_tab = new WidgetWithFlowLayout;

    QPushButton *empty_floor = create_editor_panel_button("Пустая", 70, 95, ":/img/img/EmptyCell.svg",
                                                          QSize(54, 54), floor_tab);
    CellType new_type = CellType::Empty;
    connect(empty_floor, &QPushButton::pressed, this, [this, new_type](){
        this->scene->change_selected_cells(new_type);
    });

    QPushButton *def_floor = create_editor_panel_button("Пол", 70, 95, ":/img/img/FloorCell.svg",
                                                        QSize(54, 54), floor_tab);
    new_type = CellType::Floor;
    connect(def_floor, &QPushButton::pressed, this, [this, new_type](){
        this->scene->change_selected_cells(new_type);
    });

    floor_tab->add_to_layout(empty_floor);
    floor_tab->add_to_layout(def_floor);

    QScrollArea *floor_tab_scroll_area = new QScrollArea;
    floor_tab_scroll_area->setWidgetResizable(true);
    floor_tab_scroll_area->setWidget(floor_tab);

    tabs->addTab(floor_tab_scroll_area, "Поверхности");

    // Раздел с препятствиями
    WidgetWithFlowLayout *obstacle_tab = new WidgetWithFlowLayout;

    QPushButton *def_obstacle = create_editor_panel_button("Стена", 70, 95, ":/img/img/WallWithBottom.svg",
                                                        QSize(54, 54), obstacle_tab);
    new_type = CellType::Wall;
    connect(def_obstacle, &QPushButton::pressed, this, [this, new_type](){
        this->scene->change_selected_cells(new_type);
    });

    obstacle_tab->add_to_layout(def_obstacle);

    QScrollArea *obstacle_scroll_area = new QScrollArea;
    obstacle_scroll_area->setWidgetResizable(true);
    obstacle_scroll_area->setWidget(obstacle_tab);

    tabs->addTab(obstacle_scroll_area, "Препятствия");

    // Раздел с агентами
    WidgetWithFlowLayout *agent_tab = new WidgetWithFlowLayout;

    QPushButton *def_agent = create_editor_panel_button("С обзором", 95, 95, ":/img/img/Agent.svg",
                                                        QSize(54, 54), agent_tab);
    AgentType ag_type = AgentType::LimitedView;
    connect(def_agent, &QPushButton::pressed, this, [this, ag_type](){
        this->scene->add_agent(ag_type);
    });

    agent_tab->add_to_layout(def_agent);

    QScrollArea *agent_scroll_area = new QScrollArea;
    agent_scroll_area->setWidgetResizable(true);
    agent_scroll_area->setWidget(agent_tab);

    tabs->addTab(agent_scroll_area, "Агенты");

    QWidget *editor_container = new QWidget;
    QVBoxLayout *editor_layout = new QVBoxLayout(editor_container);
    editor_layout->addWidget(tabs);

    ui->editor_panel->setWidget(editor_container);
    ui->editor_panel->setWidgetResizable(true);
}

void MainWindow::setup_settings_panel_widgets()
{
    QWidget *settings_container = new QWidget;
    QVBoxLayout *settings_layout = new QVBoxLayout;
    settings_container->setLayout(settings_layout);

    ui->rl_settings_panel->setWidget(settings_container);
    ui->rl_settings_panel->setWidgetResizable(true);
}

void MainWindow::clear_layout(QLayout *layout)
{
    if (layout == nullptr)
        return;

    QLayoutItem *item;
    while((item = layout->takeAt(0))) {
        if (item->layout()) {
            clear_layout(item->layout());
            delete item->layout();
        }
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
}

void MainWindow::setup_agent_settings(AgentObj* agent)
{
    QWidget *settings_container = new QWidget;
    QVBoxLayout *settings_layout = new QVBoxLayout;
    settings_container->setLayout(settings_layout);

    QLabel *agent_name = new QLabel;
    agent_name->setAlignment(Qt::AlignCenter);
    agent_name->setWordWrap(true);
    settings_layout->addWidget(agent_name);

    QWidget *image_widget = new QWidget;
    QVBoxLayout *image_layout = new QVBoxLayout;
    image_layout->setAlignment(Qt::AlignCenter);

    QPushButton *image_container = new QPushButton;
    image_container->setFixedSize(100, 100);
    image_container->setIconSize(QSize(95, 95));

    image_layout->addWidget(image_container);
    image_widget->setLayout(image_layout);
    settings_layout->addWidget(image_widget);

    QLabel *agent_description = new QLabel;
    agent_description->setAlignment(Qt::AlignCenter);
    agent_description->setWordWrap(true);
    settings_layout->addWidget(agent_description);

    QPushButton *agent_goal_set = new QPushButton;
    agent_goal_set->setText("Задать цель");

    QPushButton *agent_goal_remove = new QPushButton;
    agent_goal_remove->setText("Убрать цель");

    connect(agent_goal_set, &QPushButton::pressed, this, [this](){
        this->scene->set_agent_goal();
    });
    settings_layout->addWidget(agent_goal_set);

    connect(agent_goal_remove, &QPushButton::pressed, this, [this](){
        this->scene->remove_agent_goal();
    });
    settings_layout->addWidget(agent_goal_remove);

    switch (agent->get_type())
    {
    case AgentType::LimitedView:

        agent_name->setText("Агент с ограниченным обзором");
        image_container->setIcon(QIcon(":/img/img/Agent.svg"));
        agent_description->setText("Агент, который ориентируется в пространстве зная "
                                   "только о клетках, находящихся в заданном радиусе обзора.");

        QLabel *view_range = new QLabel;
        view_range->setAlignment(Qt::AlignCenter);
        view_range->setWordWrap(true);
        view_range->setText("Диапозон обзора (в клетках):");
        settings_layout->addWidget(view_range);

        QSpinBox *view_range_spin = new QSpinBox;
        view_range_spin->setWrapping(true);
        view_range_spin->setRange(1, BLOCK_LIMIT);
        view_range_spin->setValue(agent->get_view_range());
        view_range_spin->setAlignment(Qt::AlignCenter);

        connect(view_range_spin, QOverload<int>::of(&QSpinBox::valueChanged), this, [agent](int value){
            agent->set_view_range(value * SCALE_FACTOR);
        });
        settings_layout->addWidget(view_range_spin);
        break;
    }

    ui->rl_settings_panel->setWidget(settings_container);
    ui->rl_settings_panel->setWidgetResizable(true);
}

void MainWindow::setup_cell_settings(CellItem* cell)
{
    QWidget *settings_container = new QWidget;
    QVBoxLayout *settings_layout = new QVBoxLayout;
    settings_container->setLayout(settings_layout);

    QLabel *cell_name = new QLabel;
    cell_name->setAlignment(Qt::AlignCenter);
    cell_name->setWordWrap(true);
    settings_layout->addWidget(cell_name);

    QWidget *image_widget = new QWidget;
    QVBoxLayout *image_layout = new QVBoxLayout;
    image_layout->setAlignment(Qt::AlignCenter);

    QPushButton *image_container = new QPushButton;
    image_container->setFixedSize(100, 100);
    image_container->setIconSize(QSize(95, 95));

    image_layout->addWidget(image_container);
    image_widget->setLayout(image_layout);
    settings_layout->addWidget(image_widget);

    QLabel *cell_description = new QLabel;
    cell_description->setAlignment(Qt::AlignCenter);
    cell_description->setWordWrap(true);
    settings_layout->addWidget(cell_description);

    switch(cell->get_type())
    {
    case CellType::Empty:
        cell_name->setText("Пустая клетка");
        image_container->setIcon(QIcon(":/img/img/EmptyCell.svg"));
        cell_description->setText("Пустая клетка выступает в качестве фундамента для "
                                  "построения среды, в которой агент будет обучаться.");
        break;
    case CellType::Floor:
        cell_name->setText("Пол");
        image_container->setIcon(QIcon(":/img/img/FloorCell.svg"));
        cell_description->setText("Клетка пола, на которую можно поставить агента.");
        break;
    case CellType::Wall:
        cell_name->setText("Стена");
        image_container->setIcon(QIcon(":/img/img/WallWithBottom.svg"));
        cell_description->setText("Клетка стены выступает в качестве препятствия, "
                                  "которое агент не может пройти никаким образом.");
        break;
    }

    ui->rl_settings_panel->setWidget(settings_container);
    ui->rl_settings_panel->setWidgetResizable(true);
}

void MainWindow::save_scene(QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл для сохранения проекта!");
        return;
    }

    QDataStream out(&file);
    out << scene->width();
    out << scene->height();

    for (QGraphicsItem* item : scene->items())
    {
        if (auto cell = dynamic_cast<CellItem*>(item))
        {
            out << quint8(0x01);
            out << cell->get_type()
                << cell->pos();

            switch (cell->get_type())
            {
            case CellType::Empty:
                break;
            case CellType::Floor:
                break;
            case CellType::Wall:
                break;
                // Для уникальных свойств (если таковые будут)
            }
        }
    }

    for (QGraphicsItem* item : scene->items())
    {
        if (auto agent = dynamic_cast<AgentObj*>(item))
        {
            out << quint8(0x02);
            out << agent->get_type()
                << agent->pos();

            if (agent->has_goal())
                out << agent->get_goal()->pos();
            else
                out << QPointF(-10, -10);

            switch (agent->get_type())
            {
            case AgentType::LimitedView:
                out << (agent->get_view_range() * SCALE_FACTOR);
                break;
            }
        }
    }

    file.close();
}

void MainWindow::load_scene(QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл проекта!");
        return;
    }

    QDataStream in(&file);

    qreal width;
    qreal height;

    in >> width;
    in >> height;

    scene = new RL_scene(0, 0, SCALE_FACTOR, this);

    connect(scene, &RL_scene::selectionChanged, this, &MainWindow::onScene_selection_changed);
    connect(scene, &RL_scene::update_settings, this, &MainWindow::onScene_selection_changed);

    scene->setSceneRect(0, 0, width, height);
    ui->environment->setScene(scene);
    scene->fill_with_empty_cells();

    while (!in.atEnd())
    {
        quint8 marker;
        in >> marker;

        if (marker == 0x01)
        {
            CellType type;
            QPointF pos;
            in >> type >> pos;

            switch (type) {
            case CellType::Empty:
            {
                CellItem *cell = new CellItem();
                cell->setPos(pos);
                scene->load_cell(cell);
                break;
            }
            case CellType::Floor:
            {
                FloorCell *cell = new FloorCell();
                cell->setPos(pos);
                scene->load_cell(cell);
                break;
            }
            case CellType::Wall:
            {
                WallCell *cell = new WallCell();
                cell->setPos(pos);
                scene->load_cell(cell);
                break;
            }
            }
        }
        else if (marker == 0x02)
        {
            AgentType type;
            QPointF pos;
            in >> type >> pos;

            QPointF goal_pos;
            in >> goal_pos;

            CellItem *new_goal = nullptr;

            if (goal_pos.x() > 0)
            {
                new_goal = dynamic_cast<CellItem*>(scene->items(goal_pos).first());
            }

            switch (type)
            {
            case AgentType::LimitedView:
            {
                AgentObj *agent = new AgentObj();
                agent->setPos(pos);
                scene->load_agent(agent);

                int view_range;
                in >> view_range;

                agent->set_view_range(view_range);

                if (new_goal != nullptr)
                    agent->set_goal(new_goal);

                break;
            }
            }
        }
    }

    file.close();
}

void MainWindow::on_create_proj_triggered()
{
    CreateProj_Dialog w(BLOCK_LIMIT, this);

    if (w.exec() == QDialog::Accepted)
    {
        if (scene->width() > 0)
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Подтверждение", "Сохранить изменения и перейти к новому проекту?");

            if (reply == QMessageBox::Yes)
            {
                on_save_proj_triggered();
                scene = new RL_scene(0, 0, SCALE_FACTOR, this);

                connect(scene, &RL_scene::selectionChanged, this, &MainWindow::onScene_selection_changed);
                connect(scene, &RL_scene::update_settings, this, &MainWindow::onScene_selection_changed);

                scene->setSceneRect(0, 0, w.get_width() * SCALE_FACTOR, w.get_height() * SCALE_FACTOR);
                ui->environment->setScene(scene);
                scene->fill_with_empty_cells();
                return;
            }
            else
                return;
        }

        scene->setSceneRect(0, 0, w.get_width() * SCALE_FACTOR, w.get_height() * SCALE_FACTOR);

        ui->environment->setScene(scene);
        scene->fill_with_empty_cells();
    }
}


void MainWindow::on_center_navigation_triggered()
{
    if (scene->width() == 0)
    {        
        return;
    }

    ui->environment->centerOn(scene->sceneRect().center());
}


void MainWindow::on_delete_obj_triggered()
{
    if (scene->width() == 0)
    {
        return;
    }

    scene->delete_selected_objs();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (scene->width() > 0)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Подтверждение", "Сохранить изменения перед выходом?");

        if (reply == QMessageBox::Yes)
        {
            on_save_proj_triggered();
        }
    }

    QSettings settings("badin_AP-126", "RL_studio");
    settings.setValue("window_state", saveState());
    event->accept();
}

void MainWindow::onScene_selection_changed()
{
    if (this->scene->is_in_interactive_mode())
        return;

    clear_layout(ui->rl_settings_panel->widget()->layout());

    if (scene->selectedItems().size() == 0)
    {
        setup_settings_panel_widgets();
        return;
    }

    if (scene->selectedItems().size() == 1)
    {
        if (AgentObj* agent = dynamic_cast<AgentObj*>(scene->selectedItems().first()))
        {
            setup_agent_settings(agent);
        }
        else if (CellItem* cell = dynamic_cast<CellItem*>(scene->selectedItems().first()))
        {
            setup_cell_settings(cell);
        }
    }
}


void MainWindow::on_save_proj_triggered()
{
    if (scene->width() <= 0)
        return;

    if (project_path != nullptr)
    {
        qDebug() << "File saved";
        save_scene(project_path);
        return;
    }

    QString filename = QFileDialog::getSaveFileName(this, tr("Сохранить сцену"), QDir::homePath(),
                                                    tr("Бинарные файлы сцены (*.scn)"));

    if (!filename.isEmpty())
    {
        if (!filename.endsWith(".scn", Qt::CaseInsensitive))
            filename += ".scn";

        save_scene(filename);
        project_path = filename;
        qDebug() << "File saved";
    }
}


void MainWindow::on_save_as_proj_triggered()
{
    if (scene->width() <= 0)
        return;

    QString filename = QFileDialog::getSaveFileName(this, tr("Сохранить сцену"), QDir::homePath(),
                                                    tr("Бинарные файлы сцены (*.scn)"));

    if (!filename.isEmpty())
    {
        if (!filename.endsWith(".scn", Qt::CaseInsensitive))
            filename += ".scn";

        save_scene(filename);
        project_path = filename;
        qDebug() << "File saved";
    }
}


void MainWindow::on_open_proj_triggered()
{
    if (scene->width() > 0)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Подтверждение", "Сохранить изменения и перейти к новому проекту?");

        if (reply == QMessageBox::Yes)
        {
            if (project_path != nullptr)
            {
                qDebug() << "File saved";
                save_scene(project_path);
            }
            else
            {
                QString filename = QFileDialog::getSaveFileName(this, tr("Сохранить сцену"), QDir::homePath(),
                                                                tr("Бинарные файлы сцены (*.scn)"));

                if (!filename.isEmpty())
                {
                    if (!filename.endsWith(".scn", Qt::CaseInsensitive))
                        filename += ".scn";

                    save_scene(filename);
                    project_path = filename;
                    qDebug() << "File saved";
                }
            }
        }
        else
            return;
    }

    project_path = nullptr;
    QString filename = QFileDialog::getOpenFileName(this, tr("Открыть сцену"), QDir::homePath(),
                                                    tr("Бинарные файлы сцены (*.scn)"));

    if (!filename.isEmpty())
    {
        if (!filename.endsWith(".scn", Qt::CaseInsensitive))
        {
            qDebug() << "Incorrect foramt of file";
            return;
        }

        load_scene(filename);
        project_path = filename;
        scene->update_appearance();
        qDebug() << "File opened";
    }
}


void MainWindow::on_start_learning_triggered()
{
    if (!scene->is_correct_environment())
    {
        QMessageBox::warning(this, "Ошибка", "Проверьте построенную среду! (должен быть как минимум 1 агент; все клетки, кроме пустых, должны быть достижимы)");
        return;
    }

    qDebug() << "start learning";
}


void MainWindow::on_Q_learn_choice_triggered()
{
    ui->Q_learn_choice->setChecked(true);
    ui->DQN_choice->setChecked(false);
}


void MainWindow::on_DQN_choice_triggered()
{
    ui->Q_learn_choice->setChecked(false);
    ui->DQN_choice->setChecked(true);
}

void MainWindow::setup_toolbar()
{
    QComboBox *algorithms = new QComboBox;

    algorithms->addItem(ui->Q_learn_choice->text());
    algorithms->addItem(ui->DQN_choice->text());

    if (ui->Q_learn_choice->isChecked())
    {
        algorithms->setCurrentIndex(0);
    }
    else if (ui->DQN_choice->isChecked())
    {
        algorithms->setCurrentIndex(1);
    }

    ui->tool_bar->addAction(ui->center_navigation);
    ui->tool_bar->addAction(ui->start_learning);
    ui->tool_bar->addWidget(algorithms);
    ui->tool_bar->addAction(ui->delete_obj);

    connect(algorithms, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        if (index == 0)
        {
            ui->Q_learn_choice->setChecked(true);
            ui->DQN_choice->setChecked(false);
        }
        else if (index == 1)
        {
            ui->Q_learn_choice->setChecked(false);
            ui->DQN_choice->setChecked(true);
        }
    });

    connect(ui->Q_learn_choice, &QAction::triggered, this, [this, algorithms]() {
        if (ui->Q_learn_choice->isChecked())
        {
            algorithms->setCurrentIndex(0);
        }
    });

    connect(ui->DQN_choice, &QAction::triggered, this, [this, algorithms]() {
        if (ui->DQN_choice->isChecked())
        {
            algorithms->setCurrentIndex(1);
        }
    });
}

