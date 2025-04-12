#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define SCALE_FACTOR 32
#define BLOCK_LIMIT 32

#include "qdockwidget.h"
#include "environment/rl_scene.h"

#include <QMainWindow>
#include <QPushButton>

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

    void on_center_navigation_triggered();

    void on_delete_obj_triggered();

    void closeEvent(QCloseEvent *event) override;

    void onScene_selection_changed();

private:
    RL_scene *scene;
    QDockWidget *editor_dock;
    QDockWidget *log_dock;
    QDockWidget *settings_dock;

    Ui::MainWindow *ui;
    void setup_widgets();
    QPushButton* create_editor_panel_button(QString btn_name, int btn_w, int btn_h, QString icon_path, QSize icon_size, QWidget *parent);
    void setup_editor_panel_widgets();
    void setup_settings_panel_widgets();
    void clear_layout(QLayout* layout);
    void setup_agent_settings(AgentObj* agent);
    void setup_cell_settings(CellItem* cell);
};
#endif // MAINWINDOW_H
