#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "createproj_dialog.h"
#include "widgetwithflowlayout.h"

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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , scene(new RL_scene(0, 0, SCALE_FACTOR, this))
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setup_widgets();
    setup_editor_panel_widgets();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setup_widgets()
{
    QWidget *central_widget = new QWidget(this);
    setCentralWidget(central_widget);

    QDockWidget *editor_dock = new QDockWidget("Редактор", this);
    QDockWidget *log_dock = new QDockWidget("Логи", this);
    QDockWidget *settings_dock = new QDockWidget("Настройки", this);

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

    QWidget *editor_container = new QWidget;
    QVBoxLayout *editor_layout = new QVBoxLayout(editor_container);
    editor_layout->addWidget(tabs);

    ui->editor_panel->setWidget(editor_container);
    ui->editor_panel->setWidgetResizable(true);
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

