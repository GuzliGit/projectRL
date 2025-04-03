#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define SCALE_FACTOR 32
#define BLOCK_LIMIT 32
#include <QMainWindow>
#include "rl_scene.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_create_proj_triggered();

private:
    RL_scene *scene;

    Ui::MainWindow *ui;
    void setupWidgets();
};
#endif // MAINWINDOW_H
