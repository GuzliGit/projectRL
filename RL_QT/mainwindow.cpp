#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDockWidget>
#include <QLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
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

    ui->environment->setScene(new QGraphicsScene(this));
    setCentralWidget(ui->environment);
}

MainWindow::~MainWindow()
{
    delete ui;
}
