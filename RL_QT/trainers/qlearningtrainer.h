#ifndef Q_LEARNING_TRAINER_H
#define Q_LEARNING_TRAINER_H

#include "environment/rl_scene.h"

#include <QObject>

class QLearningTrainer : public QObject
{
    Q_OBJECT
public:
    QLearningTrainer(RL_scene *scene) : m_scene(scene) {};

public slots:
    void start_training(double alpha_t, double gamma_t, double epsilon_t, int episodes_count);
    void cancel_training() { cancel_requested = true; }

signals:
    void update_logs(QVector<int> rewards, int episode_num);
    void training_finished(QVector<QVector<int>> rewards, QPointF *coords);
    void scene_disabled(bool val);
    void save_learning(QVector<double**> q_tabs, QList<AgentObj*> agents);

private:
    RL_scene *m_scene;

    std::atomic<bool> cancel_requested = false;
};

#endif
