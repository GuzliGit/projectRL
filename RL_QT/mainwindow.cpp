#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "createproj_dialog.h"

#include <QDockWidget>
#include <QLayout>
#include <QDialog>
#include <QMessageBox>
#include <QDebug>
#include <QPainter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , scene(new RL_scene(0, 0, SCALE_FACTOR, this))
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupWidgets();
}

void MainWindow::setupWidgets()
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

MainWindow::~MainWindow()
{
    delete ui;
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
        QMessageBox::warning(this, "Предупреждение", "Для навигации необходимо создать графическую среду!");
        return;
    }

    ui->environment->centerOn(scene->sceneRect().center());
}

