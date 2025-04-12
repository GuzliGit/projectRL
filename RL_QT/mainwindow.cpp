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

#define PIXMAP_INDENT 10

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , scene(new RL_scene(0, 0, SCALE_FACTOR, this))
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setup_widgets();
    setup_editor_panel_widgets();
    setup_settings_panel_widgets();

    QSettings settings("badin_AP-126", "RL_studio");
    restoreState(settings.value("window_state").toByteArray());

    connect(scene, &RL_scene::selectionChanged, this, &MainWindow::onScene_selection_changed);
}

MainWindow::~MainWindow()
{
    delete ui;
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

    switch (agent->get_type())
    {
    case AgentType::LimitedView:
        QLabel *agent_name = new QLabel;
        agent_name->setAlignment(Qt::AlignCenter);
        agent_name->setText("Агент с ограниченным обзором");
        settings_layout->addWidget(agent_name);

        QWidget *image_container = new QWidget;
        QHBoxLayout *image_layout = new QHBoxLayout(image_container);
        image_layout->setAlignment(Qt::AlignCenter);

        QLabel *image_label = new QLabel;
        image_label->setPixmap(QPixmap(":/img/img/Agent.svg"));
        image_label->setFixedSize(QSize(agent->get_width() + PIXMAP_INDENT,
                                        agent->get_height() + PIXMAP_INDENT));
        image_label->setAlignment(Qt::AlignCenter);

        image_layout->addWidget(image_label);
        image_layout->setContentsMargins(0, 0, 0, 0);
        settings_layout->addWidget(image_container);

        QLabel *view_range = new QLabel;
        view_range->setAlignment(Qt::AlignCenter);
        view_range->setText("Диапозон обзора (в клетках):");
        settings_layout->addWidget(view_range);

        QSpinBox *view_range_spin = new QSpinBox;
        view_range_spin->setRange(1, BLOCK_LIMIT);
        view_range_spin->setValue(agent->get_view_range());
        view_range_spin->setAlignment(Qt::AlignCenter);

        connect(view_range_spin, QOverload<int>::of(&QSpinBox::valueChanged), [agent](int value){
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
                qDebug() << "Сохранение текущего проекта и создание нового";
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
    QSettings settings("badin_AP-126", "RL_studio");
    settings.setValue("window_state", saveState());
    event->accept();
}

void MainWindow::onScene_selection_changed()
{
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

