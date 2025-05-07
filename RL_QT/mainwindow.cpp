#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "createproj_dialog.h"
#include "custom_tools/widgetwithflowlayout.h"
#include "agent/agentobj.h"
#include "environment/cellitem.h"
#include "environment/floorcell.h"
#include "environment/wallcell.h"
#include "environment/gatecell.h"
#include "environment/riskycell.h"

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
#include <QFormLayout>
#include <QtConcurrent/QtConcurrent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , scene(new RL_scene(0, 0, SCALE_FACTOR, this))
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setup_toolbar();
    setup_logs();
    setup_widgets();
    setup_editor_panel_widgets();
    setup_settings_panel_widgets();
    setup_learning_panel_widgets();

    algorithms->setCurrentIndex(0);
    setup_q_learn_panel();

    training_thread = new QThread(this);

    QSettings settings("badin_AP-126", "RL_studio");
    restoreState(settings.value("window_state").toByteArray());

    connect(scene, &RL_scene::selectionChanged, this, &MainWindow::onScene_selection_changed);
    connect(scene, &RL_scene::update_settings, this, &MainWindow::onScene_selection_changed);
    connect(scene, &RL_scene::learning_finished, this, &MainWindow::display_learning_charts);
    connect(scene, &RL_scene::update_logs, this, &MainWindow::display_all_learning_logs);
    connect(scene, &RL_scene::save_learning, this, &MainWindow::save_agents_q_tabs);
}

MainWindow::~MainWindow()
{
    training_thread->quit();
    training_thread->wait();

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

    learning_dock = new QDockWidget("Обучение", this);
    learning_dock->setObjectName("learn");

    editor_dock->setWidget(ui->editor_panel);
    log_dock->setWidget(ui->log_panel);
    settings_dock->setWidget(ui->rl_settings_panel);
    learning_dock->setWidget(ui->learning_panel);

    addDockWidget(Qt::LeftDockWidgetArea, editor_dock);
    addDockWidget(Qt::BottomDockWidgetArea, log_dock);
    addDockWidget(Qt::RightDockWidgetArea, settings_dock);
    addDockWidget(Qt::BottomDockWidgetArea, learning_dock);

    editor_dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetFloatable);
    log_dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetFloatable);
    settings_dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetFloatable);
    learning_dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetFloatable);

    setCentralWidget(ui->environment);
}

QPushButton * MainWindow::create_editor_panel_button(QString btn_name, int btn_w, int btn_h, QString icon_path, QSize icon_size, QWidget *parent)
{
    QVBoxLayout *new_layout = new QVBoxLayout;
    new_layout->setSpacing(5);
    new_layout->setContentsMargins(5, 5, 5, 5);

    QPushButton *btn = new QPushButton(parent);
    btn->setFixedSize(btn_w, btn_h);
    btn->setIcon(QIcon(icon_path));
    btn->setIconSize(icon_size);
    btn->setLayout(new_layout);

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

    QPushButton *gate_obstacle = create_editor_panel_button("Дин. стена", 70, 95, ":/img/img/GateCellFree.svg",
                                                            QSize(54, 54), obstacle_tab);
    new_type = CellType::Gate;
    connect(gate_obstacle, &QPushButton::pressed, this, [this, new_type](){
        this->scene->change_selected_cells(new_type);
    });

    obstacle_tab->add_to_layout(gate_obstacle);

    QPushButton *risky_obstacle = create_editor_panel_button("Риск", 70, 95, ":/img/img/RiskyCellFree.svg",
                                                             QSize(54, 54), obstacle_tab);
    new_type = CellType::Risky;
    connect(risky_obstacle, &QPushButton::pressed, this, [this, new_type](){
        this->scene->change_selected_cells(new_type);
    });

    obstacle_tab->add_to_layout(risky_obstacle);

    QScrollArea *obstacle_scroll_area = new QScrollArea;
    obstacle_scroll_area->setWidgetResizable(true);
    obstacle_scroll_area->setWidget(obstacle_tab);

    tabs->addTab(obstacle_scroll_area, "Препятствия");

    // Раздел с агентами
    WidgetWithFlowLayout *agent_tab = new WidgetWithFlowLayout;

    QPushButton *def_agent = create_editor_panel_button("Базовый", 95, 95, ":/img/img/Agent.svg",
                                                        QSize(54, 54), agent_tab);
    AgentType ag_type = AgentType::Default;
    connect(def_agent, &QPushButton::pressed, this, [this, ag_type](){
        this->scene->add_agent(ag_type);
    });

    agent_tab->add_to_layout(def_agent);

    QScrollArea *agent_scroll_area = new QScrollArea;
    agent_scroll_area->setWidgetResizable(true);
    agent_scroll_area->setWidget(agent_tab);

    tabs->addTab(agent_scroll_area, "Агенты");

    QWidget *editor_container = new QWidget;
    editor_container->setObjectName("editor_container");
    QVBoxLayout *editor_layout = new QVBoxLayout(editor_container);
    editor_layout->addWidget(tabs);

    ui->editor_panel->setWidget(editor_container);
    ui->editor_panel->setWidgetResizable(true);
}

void MainWindow::setup_settings_panel_widgets()
{
    QWidget *settings_container = new QWidget;
    settings_container->setObjectName("settings_container");
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
    settings_container->setObjectName("settings_container");
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

    QLabel *learn_file_label = new QLabel;
    learn_file_label->setAlignment(Qt::AlignCenter);
    learn_file_label->setWordWrap(true);
    learn_file_label->setText("Файл обучения:");
    settings_layout->addWidget(learn_file_label);

    QTextEdit *learn_file_path = new QTextEdit;
    learn_file_path->setAlignment(Qt::AlignCenter);
    learn_file_path->setFixedHeight(learn_file_path->fontMetrics().height() * 3 + 20);
    learn_file_path->setWordWrapMode(QTextOption::WordWrap);
    learn_file_path->setReadOnly(true);
    learn_file_path->setText(agent->get_learning_file_path());
    settings_layout->addWidget(learn_file_path);

    QPushButton *learn_file_set = new QPushButton;
    learn_file_set->setText("Задать файл");

    QPushButton *learn_file_remove = new QPushButton;
    learn_file_remove->setText("Убрать файл");

    connect(learn_file_set, &QPushButton::pressed, this, [this, agent, learn_file_path](){
        this->load_agent_learning(agent);
        learn_file_path->setText(agent->get_learning_file_path());
    });
    settings_layout->addWidget(learn_file_set);

    connect(learn_file_remove, &QPushButton::pressed, agent, [agent, learn_file_path](){
        agent->delete_learning_file();
        learn_file_path->setText(agent->get_learning_file_path());
    });
    settings_layout->addWidget(learn_file_remove);

    switch (agent->get_type())
    {
    case AgentType::Default:

        agent_name->setText("Базовая реализация агента");
        image_container->setIcon(QIcon(":/img/img/Agent.svg"));
        agent_description->setText("Агент, который ориентируется в пространстве, зная "
                                   "о расположении клеток заранее.");
        break;
    }

    ui->rl_settings_panel->setWidget(settings_container);
    ui->rl_settings_panel->setWidgetResizable(true);
}

void MainWindow::setup_cell_settings(CellItem* cell)
{
    QWidget *settings_container = new QWidget;
    settings_container->setObjectName("settings_container");
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
    case CellType::Gate:
    {
        cell_name->setText("Динамическая стена");
        image_container->setIcon(QIcon(":/img/img/GateCellFree.svg"));
        cell_description->setText("Клетка динамической стены выступает в качестве динамического препятствия, которое с некоторой вероятностью может быть перекрыто."
                                  "Агент может пройти только в момент, когда данная клетка не перекрыта.");

        QLabel *closing_spin_label = new QLabel;
        closing_spin_label->setWordWrap(true);
        closing_spin_label->setAlignment(Qt::AlignCenter);
        closing_spin_label->setText("Шанс перекрытия клетки");
        settings_layout->addWidget(closing_spin_label);

        GateCell *gate_cell = dynamic_cast<GateCell*>(cell);

        QSpinBox* closing_chance_spin = new QSpinBox();
        closing_chance_spin->setWrapping(true);
        closing_chance_spin->setRange(1, 90);
        closing_chance_spin->setValue(gate_cell->get_closing_chance());
        closing_chance_spin->setAlignment(Qt::AlignCenter);

        connect(closing_chance_spin, &QSpinBox::valueChanged, gate_cell, [gate_cell](int value){
            gate_cell->set_closing_chance(value);
        });

        settings_layout->addWidget(closing_chance_spin);
        break;
    }
    case CellType::Risky:
    {
        cell_name->setText("Рискованная клетка");
        image_container->setIcon(QIcon(":/img/img/RiskyCellFree.svg"));
        cell_description->setText("Рискованная клетка выступает в качестве динамического препятствия, в которой с некоторой вероятностью может застрять агент.");

        QLabel *stucked_spin_label = new QLabel;
        stucked_spin_label->setWordWrap(true);
        stucked_spin_label->setAlignment(Qt::AlignCenter);
        stucked_spin_label->setText("Шанс застревания агента");
        settings_layout->addWidget(stucked_spin_label);

        RiskyCell *risky_cell = dynamic_cast<RiskyCell*>(cell);

        QSpinBox* stuck_chance_spin = new QSpinBox();
        stuck_chance_spin->setWrapping(true);
        stuck_chance_spin->setRange(1, 90);
        stuck_chance_spin->setValue(risky_cell->get_stuck_chance());
        stuck_chance_spin->setAlignment(Qt::AlignCenter);

        connect(stuck_chance_spin, &QSpinBox::valueChanged, risky_cell, [risky_cell](int val){
            risky_cell->set_stuck_chance(val);
        });

        settings_layout->addWidget(stuck_chance_spin);
    }
    }

    ui->rl_settings_panel->setWidget(settings_container);
    ui->rl_settings_panel->setWidgetResizable(true);
}

void MainWindow::setup_learning_panel_widgets()
{
    QWidget *learning_container = new QWidget;
    learning_container->setObjectName("learning_container");
    QHBoxLayout *learning_layout = new QHBoxLayout;
    learning_container->setLayout(learning_layout);

    ui->learning_panel->setWidget(learning_container);
    ui->learning_panel->setWidgetResizable(true);
}

void MainWindow::setup_q_learn_panel()
{
    QWidget *learning_container = new QWidget;
    learning_container->setObjectName("learning_container");
    QHBoxLayout *learning_layout = new QHBoxLayout;
    learning_container->setLayout(learning_layout);

    // Левая панель
    QScrollArea *parameters_area = new QScrollArea();
    QWidget *parameters_widget = new QWidget();
    QVBoxLayout *form_layout = new QVBoxLayout(parameters_widget);

    QLabel *episodes_label = new QLabel;
    episodes_label->setAlignment(Qt::AlignCenter);
    episodes_label->setWordWrap(true);
    episodes_label->setText("Эпизоды:");
    form_layout->addWidget(episodes_label);

    episode_spin = new QSpinBox();
    episode_spin->setWrapping(true);
    episode_spin->setRange(1, MAX_EPISODES);
    episode_spin->setValue(100);
    episode_spin->setAlignment(Qt::AlignCenter);
    form_layout->addWidget(episode_spin);

    QLabel *alpha_label = new QLabel;
    alpha_label->setAlignment(Qt::AlignCenter);
    alpha_label->setWordWrap(true);
    alpha_label->setText("Альфа:");
    form_layout->addWidget(alpha_label);

    alpha_spin = new QDoubleSpinBox();
    alpha_spin->setWrapping(true);
    alpha_spin->setRange(0.05, 1.0);
    alpha_spin->setSingleStep(0.05);
    alpha_spin->setValue(0.3);
    alpha_spin->setAlignment(Qt::AlignCenter);
    form_layout->addWidget(alpha_spin);

    QLabel *gamma_label = new QLabel;
    gamma_label->setAlignment(Qt::AlignCenter);
    gamma_label->setWordWrap(true);
    gamma_label->setText("Гамма:");
    form_layout->addWidget(gamma_label);

    gamma_spin = new QDoubleSpinBox();
    gamma_spin->setWrapping(true);
    gamma_spin->setRange(0.05, 1.0);
    gamma_spin->setSingleStep(0.05);
    gamma_spin->setValue(0.9);
    gamma_spin->setAlignment(Qt::AlignCenter);
    form_layout->addWidget(gamma_spin);

    QLabel *start_eps_label = new QLabel;
    start_eps_label->setAlignment(Qt::AlignCenter);
    start_eps_label->setWordWrap(true);
    start_eps_label->setText("Начальный эпсилон:");
    form_layout->addWidget(start_eps_label);

    start_epsilon_spin = new QDoubleSpinBox();
    start_epsilon_spin->setWrapping(true);
    start_epsilon_spin->setRange(0.05, 1.0);
    start_epsilon_spin->setSingleStep(0.05);
    start_epsilon_spin->setValue(1.0);
    start_epsilon_spin->setAlignment(Qt::AlignCenter);
    form_layout->addWidget(start_epsilon_spin);

    parameters_area->setWidget(parameters_widget);
    parameters_area->setWidgetResizable(true);

    // Правая панель
    charts_tab = new QTabWidget();

    learning_layout->addWidget(parameters_area, 1);
    learning_layout->addWidget(charts_tab, 3);

    ui->learning_panel->setWidget(learning_container);
    ui->learning_panel->setWidgetResizable(true);
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
            case CellType::Gate:
            {
                if (GateCell* item = dynamic_cast<GateCell*>(cell))
                    out << item->get_closing_chance();
                break;
            }
            case CellType::Risky:
            {
                if (RiskyCell* item = dynamic_cast<RiskyCell*>(cell))
                {
                    out << item->get_stuck_chance();
                }
                break;
            }
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
            case AgentType::Default:
                //out << (agent->get_view_range() * SCALE_FACTOR);
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

    scene->clear_selection();
    charts_tab->clear();
    if (scene->is_visualize_status())
        on_visualize_learning_triggered();

    scene = new RL_scene(0, 0, SCALE_FACTOR, this);

    connect(scene, &RL_scene::selectionChanged, this, &MainWindow::onScene_selection_changed);
    connect(scene, &RL_scene::update_settings, this, &MainWindow::onScene_selection_changed);
    connect(scene, &RL_scene::learning_finished, this, &MainWindow::display_learning_charts);
    connect(scene, &RL_scene::update_logs, this, &MainWindow::display_all_learning_logs);
    connect(scene, &RL_scene::save_learning, this, &MainWindow::save_agents_q_tabs);

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
            case CellType::Gate:
            {
                GateCell *cell = new GateCell();
                cell->setPos(pos);
                int closing_chance;
                in >> closing_chance;
                cell->set_closing_chance(closing_chance);
                scene->load_cell(cell);
                break;
            }
            case CellType::Risky:
            {
                RiskyCell *cell = new RiskyCell();
                cell->setPos(pos);
                int stuck_chance;
                in >> stuck_chance;
                cell->set_stuck_chance(stuck_chance);
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

            if (goal_pos.x() >= 0)
            {
                new_goal = dynamic_cast<CellItem*>(scene->items(goal_pos).first());
            }

            switch (type)
            {
            case AgentType::Default:
            {
                AgentObj *agent = new AgentObj();
                agent->setPos(pos);
                scene->load_agent(agent);

                if (new_goal != nullptr)
                    agent->set_goal(new_goal);

                break;
            }
            }
        }
    }

    file.close();
}

void MainWindow::save_agents_q_tabs(QVector<double**> q_tables,  QList<AgentObj*> agents)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Подтверждение", "Сохранить прогресс агентов?");

    if (reply == QMessageBox::No)
        return;

    int agents_with_file = 0;
    for (auto agent : agents)
    {
        if (agent->has_learning_file())
            agents_with_file++;
    }

    QString dir_path;

    if (agents_with_file != agents.size())
    {
        dir_path = QFileDialog::getExistingDirectory(this, tr("Выберите директорию для сохранения"), QDir::homePath(),
                                                             QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

        if (dir_path.isEmpty())
            return;
    }

    qreal width = scene->width();
    qreal height = scene->height();
    int state_size = scene->width() / SCALE_FACTOR * scene->height() / SCALE_FACTOR;
    int actions_size = 4;

    QDateTime current_time = QDateTime::currentDateTime();
    QString timestamp = current_time.toString("yyyyMMdd_hhmmss");

    for (int i = 0; i < agents.size(); i++)
    {
        QString filename;

        if (!agents[i]->has_learning_file())
        {
            QString file = QString("agent%1_%2.qtab")
                                   .arg(i, 2, 10, QLatin1Char('0'))
                                   .arg(timestamp);
            filename = dir_path + "/" + file;
        }
        else
        {
            filename = agents[i]->get_learning_file_path();
            qDebug() << filename << i;
        }

        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::warning(this, "Ошибка", QString("Не удалось создать файл для агента %1").arg(i));
            continue;
        }

        QDataStream out(&file);

        out << width;
        out << height;

        double** q_table = q_tables[i];
        for (int s = 0; s < state_size; ++s)
        {
            for (int a = 0; a < actions_size; ++a)
            {
                out << q_table[s][a];
            }
        }

        agents[i]->set_q_table(q_table, state_size, actions_size);
        agents[i]->set_learning_file(filename);

        file.close();
    }
}

void MainWindow::load_agent_learning(AgentObj* agent)
{
    bool is_q_tab = false;
    QString filename = QFileDialog::getOpenFileName(this, tr("Загрузить файл обучения"), QDir::homePath(),
                                                    tr("Бинарные файлы табличных алгоритмов (*.qtab)"));

    if (!filename.isEmpty())
    {
        if (filename.endsWith(".qtab", Qt::CaseInsensitive))
        {
            is_q_tab = true;
        }
        else
        {
            qDebug() << "Incorrect foramt of file";
            return;
        }

        QFile file(filename);
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

        if (scene->width() != width || scene->height() != height)
        {
            QMessageBox::warning(this, "Ошибка", "Агент должен быть обучен в среде тех же размеров!");
            file.close();
            return;
        }

        if (is_q_tab)
        {
            int states_num = (scene->width() / SCALE_FACTOR * scene->height() / SCALE_FACTOR);
            int actions_size = 4;

            double **q = new double*[states_num];
            for (int i = 0; i < states_num; i++) {
                q[i] = new double[actions_size];
                for (int j = 0; j < actions_size; j++) {
                    in >> q[i][j];
                }
            }

            agent->set_q_table(q, states_num, actions_size);
            agent->set_learning_file(filename);

            for (int i = 0; i < states_num; i++)
                delete[] q[i];

            delete[] q;
        }
    }
}

void MainWindow::set_ui_enabled(bool val)
{
    ui->rl_settings_panel->setEnabled(val);
    ui->editor_panel->setEnabled(val);
    ui->create_proj->setEnabled(val);
    ui->save_proj->setEnabled(val);
    ui->save_as_proj->setEnabled(val);
    ui->open_proj->setEnabled(val);

    ui->center_navigation->setEnabled(val);
    ui->start_learning->setEnabled(val);
    ui->Q_learn_choice->setEnabled(val);
    ui->DQN_choice->setEnabled(val);
    ui->visualize_learning->setEnabled(val);
    ui->delete_obj->setEnabled(val);

    ui->stop_learning->setEnabled(!val);

    algorithms->setEnabled(val);
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
                scene->clear_selection();
                charts_tab->clear();
                if (scene->is_visualize_status())
                    on_visualize_learning_triggered();

                on_save_proj_triggered();
                scene = new RL_scene(0, 0, SCALE_FACTOR, this);

                connect(scene, &RL_scene::selectionChanged, this, &MainWindow::onScene_selection_changed);
                connect(scene, &RL_scene::update_settings, this, &MainWindow::onScene_selection_changed);
                connect(scene, &RL_scene::learning_finished, this, &MainWindow::display_learning_charts);
                connect(scene, &RL_scene::update_logs, this, &MainWindow::display_all_learning_logs);
                connect(scene, &RL_scene::save_learning, this, &MainWindow::save_agents_q_tabs);

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
        QMessageBox::warning(this, "Ошибка", "Проверьте построенную среду! (должен быть как минимум 1 агент; все клетки, кроме пустых, должны быть достижимы; целевые клетки не должны перекрывать друг друга)");
        return;
    }

    TrainAlgorithms algorithm = (TrainAlgorithms)algorithms->currentIndex();
    switch (algorithm) {
    case TrainAlgorithms::QLearn:
    {
        if (scene->is_visualize_status())
        { 
            trainer = new QLearningTrainer(scene);            

            scene->set_training(true);
            scene->prepare_scene_objs();
            set_ui_enabled(false);

            trainer->moveToThread(training_thread);

            connect(this, &MainWindow::cancel_requested, trainer, &QLearningTrainer::cancel_training, Qt::DirectConnection);
            connect(trainer, &QLearningTrainer::update_logs, this, &MainWindow::display_learning_logs_by_step);
            connect(trainer, &QLearningTrainer::training_finished, this, &MainWindow::display_learning_charts);
            connect(trainer, &QLearningTrainer::scene_disabled, scene, &RL_scene::set_training);
            connect(trainer, &QLearningTrainer::save_learning, this, &MainWindow::save_agents_q_tabs, Qt::BlockingQueuedConnection);

            training_thread->start();

            QMetaObject::invokeMethod(trainer, "start_training",
                                      Q_ARG(double, alpha_spin->value()),
                                      Q_ARG(double, gamma_spin->value()),
                                      Q_ARG(double, start_epsilon_spin->value()),
                                      Q_ARG(int, episode_spin->value()));
        }
        else
        {
            ui->log_panel->clear();
            ui->log_panel->appendPlainText(QString("Начало обучения: [%1]").arg(QDateTime::currentDateTime().toString()));

            scene->prepare_scene_objs();
            scene->start_qlearn(alpha_spin->value(), gamma_spin->value(), start_epsilon_spin->value(), episode_spin->value());
        }

        break;
    }
    default:
        break;
    }
}


void MainWindow::on_Q_learn_choice_triggered()
{
    ui->Q_learn_choice->setChecked(true);
    ui->DQN_choice->setChecked(false);

    clear_layout(ui->learning_panel->widget()->layout());
    setup_q_learn_panel();
}


void MainWindow::on_DQN_choice_triggered()
{
    ui->Q_learn_choice->setChecked(false);
    ui->DQN_choice->setChecked(true);

    clear_layout(ui->learning_panel->widget()->layout());
}

void MainWindow::setup_toolbar()
{
    algorithms = new QComboBox;

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
    ui->tool_bar->addAction(ui->stop_learning);
    ui->tool_bar->addAction(ui->visualize_learning);
    ui->tool_bar->addAction(ui->delete_obj);

    ui->stop_learning->setEnabled(false);

    connect(algorithms, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        if (index == 0)
        {
            ui->Q_learn_choice->setChecked(true);
            ui->DQN_choice->setChecked(false);

            clear_layout(ui->learning_panel->widget()->layout());
            setup_q_learn_panel();
        }
        else if (index == 1)
        {
            ui->Q_learn_choice->setChecked(false);
            ui->DQN_choice->setChecked(true);

            clear_layout(ui->learning_panel->widget()->layout());
        }
    });

    connect(ui->Q_learn_choice, &QAction::triggered, this, [this]() {
        if (ui->Q_learn_choice->isChecked())
        {
            algorithms->setCurrentIndex(0);
        }
    });

    connect(ui->DQN_choice, &QAction::triggered, this, [this]() {
        if (ui->DQN_choice->isChecked())
        {
            algorithms->setCurrentIndex(1);
        }
    });
}

void MainWindow::setup_logs()
{
    int font_id = QFontDatabase::addApplicationFont(":/font/fonts/Roboto-Bold.ttf");;
    QStringList font_families = QFontDatabase::applicationFontFamilies(font_id);
    if (!font_families.isEmpty())
    {
        QString font_family = font_families.at(0);

        ui->log_panel->setStyleSheet(
            QString("QPlainTextEdit {"
                    "    font-family: '%1';"
                    "    font-weight: bold;"
                    "    font-size: 16pt;"
                    "}"
                    ).arg(font_family)
            );
    }

    ui->log_panel->setReadOnly(true);
}


void MainWindow::on_visualize_learning_triggered()
{
    if (scene->width() == 0)
        return;

    if (scene->is_visualize_status())
    {
        ui->visualize_learning->setIcon(QIcon(":/img/img/ClosedEye.svg"));
        scene->set_visualize_status(false);
    }
    else
    {
        ui->visualize_learning->setIcon(QIcon(":/img/img/OpenedEye.svg"));
        scene->set_visualize_status(true);
    }
}

void MainWindow::display_learning_charts(QVector<QVector<int> > rewards, QPointF *coords)
{
    set_ui_enabled(true);
    ui->log_panel->appendPlainText(QString("Конец обучения: [%1]").arg(QDateTime::currentDateTime().toString()));
    charts_tab->clear();

    for (int i = 0; i < rewards.size(); ++i)
    {
        QVector<int> agent_rewards = rewards[i];

        QLineSeries *series = new QLineSeries();
        for (int j = 0; j < agent_rewards.size(); ++j)
        {
            series->append(j, agent_rewards[j]);
        }

        QChart *chart = new QChart();
        chart->addSeries(series);
        chart->setTitle(QString("Агент %1 {%2, %3}: Награда по эпизодам").arg(i + 1).arg(coords[i].x() + 1).arg(coords[i].y() + 1));
        chart->createDefaultAxes();
        chart->axes(Qt::Vertical).first()->setTitleText("Награда");
        chart->axes(Qt::Horizontal).first()->setTitleText("Эпизод");

        QChartView *chart_view = new QChartView(chart);
        chart_view->setRenderHint(QPainter::Antialiasing);

        charts_tab->addTab(chart_view, QString("Агент %1").arg(i + 1));
    }
}

void MainWindow::display_learning_logs_by_step(QVector<int> rewards, int episode_num)
{
    if (episode_num == 0)
    {
        ui->log_panel->clear();
        ui->log_panel->appendPlainText(QString("Начало обучения: [%1]").arg(QDateTime::currentDateTime().toString()));
    }

    for (int i = 0; i < rewards.size(); i++)
    {
        ui->log_panel->appendPlainText(QString("Агент %1 | Эпизод %2: %3").arg(i + 1).arg(episode_num + 1).arg(rewards[i]));
    }
}

void MainWindow::display_all_learning_logs(QVector<QVector<int> > rewards)
{
    for (int i = 0; i < rewards.size(); i++)
    {
        for (int j = 0; j < rewards[i].size(); j++)
        {
            ui->log_panel->appendPlainText(QString("Агент %1 | Эпизод %2: %3").arg(i + 1).arg(j + 1).arg(rewards[i][j]));
        }
    }
}


void MainWindow::on_stop_learning_triggered()
{
    ui->log_panel->appendPlainText(QString("Преждевременная остановка процесса обучения [%1]").arg(QDateTime::currentDateTime().toString()));
    ui->stop_learning->setEnabled(false);
    emit cancel_requested();
}

