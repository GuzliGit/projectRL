#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define SCALE_FACTOR 32
#define BLOCK_LIMIT 32
#define MAX_EPISODES 10000

#include "qdockwidget.h"
#include "environment/rl_scene.h"

#include <QMainWindow>
#include <QPushButton>
#include <QComboBox>
#include <QtCharts/QtCharts>

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

protected:
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void on_create_proj_triggered();

    void on_center_navigation_triggered();

    void on_delete_obj_triggered();

    void closeEvent(QCloseEvent *event) override;

    void onScene_selection_changed();

    void on_save_proj_triggered();

    void on_save_as_proj_triggered();

    void on_open_proj_triggered();

    void on_start_learning_triggered();

    void on_Q_learn_choice_triggered();

    void on_DQN_choice_triggered();

    void on_visualize_learning_triggered();

    void display_learning_charts(QVector<QVector<int>> rewards);

signals:
    void click_in_interactive_mode();

private:
    RL_scene *scene;
    QDockWidget *editor_dock;
    QDockWidget *log_dock;
    QDockWidget *settings_dock;
    QDockWidget *learning_dock;

    QSpinBox *episode_spin;
    QDoubleSpinBox *alpha_spin;
    QDoubleSpinBox *gamma_spin;
    QDoubleSpinBox *start_epsilon_spin;
    QTabWidget *charts_tab;

    QComboBox *algorithms;
    QString project_path = nullptr;

    Ui::MainWindow *ui;
    void setup_toolbar();
    void setup_widgets();
    QPushButton* create_editor_panel_button(QString btn_name, int btn_w, int btn_h, QString icon_path, QSize icon_size, QWidget *parent);
    void setup_editor_panel_widgets();
    void setup_settings_panel_widgets();
    void clear_layout(QLayout* layout);
    void setup_agent_settings(AgentObj* agent);
    void setup_cell_settings(CellItem* cell);
    void setup_learning_panel_widgets();
    void setup_q_learn_panel();
    void save_scene(QString &path);
    void load_scene(QString &path);
};
#endif // MAINWINDOW_H
