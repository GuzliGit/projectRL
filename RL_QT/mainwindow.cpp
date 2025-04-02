#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSplitter>
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
    QSplitter *main_splitter = new QSplitter(Qt::Horizontal, this);
    setCentralWidget(main_splitter);

    // Панель с элементами для графической среды (подобие панели виджетов)
    QVBoxLayout *left_layout = new QVBoxLayout(ui->editor_panel);

    // Панель с графической средой и логами
    QSplitter *center_splitter = new QSplitter(Qt::Vertical, this);
    QVBoxLayout *center_layout = new QVBoxLayout();

    center_layout->addWidget(ui->environment);
    ui->environment->setScene(new QGraphicsScene());
    center_layout->addWidget(ui->log_panel);

    center_splitter->addWidget(ui->environment);
    center_splitter->addWidget(ui->log_panel);

    // Панель с настройками среды и графиками обучения
    QVBoxLayout *right_panel = new QVBoxLayout(ui->rl_settings_panel);

    // // Работа со сплиттером
    main_splitter->addWidget(ui->editor_panel);
    main_splitter->addWidget(center_splitter);
    main_splitter->addWidget(ui->rl_settings_panel);
}

MainWindow::~MainWindow()
{
    delete ui;
}
