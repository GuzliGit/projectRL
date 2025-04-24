#include "trainers/qlearningtrainer.h"
#include "mainwindow.h"
#include "algorithms/q_learn_exp.h"

void QLearningTrainer::start_training(double alpha_t, double gamma_t, double epsilon_t, int episodes_count)
{
    const short state_size = m_scene->width() / SCALE_FACTOR * m_scene->height() / SCALE_FACTOR;
    const short agents_count = m_scene->all_agents.size();

    QVector<QVector<int>> total_rewards(agents_count);
    QVector<int> episode_rewards(agents_count);
    QPointF agents_coords[agents_count];

    for (int i = 0; i < agents_count; i++)
    {
        total_rewards[i] = QVector<int>(episodes_count);
        agents_coords[i] = QPointF(m_scene->all_agents[i]->pos().x() / SCALE_FACTOR, m_scene->all_agents[i]->pos().y() / SCALE_FACTOR);
        for (int j = 0; j < episodes_count; j++)
        {
            total_rewards[i][j] = 0;
        }
    }

    QMetaObject::invokeMethod(m_scene, "prepare_for_learning", Qt::BlockingQueuedConnection);
    init_qlearn(state_size, agents_count, alpha_t, gamma_t, epsilon_t);

    short states[agents_count];
    char actions[agents_count];
    short next_states[agents_count];
    signed char rewards[agents_count];
    char dones[agents_count];

    for (int k = 0; k < episodes_count; k++)
    {
        QMetaObject::invokeMethod(m_scene, "reset_env", Qt::BlockingQueuedConnection);
        reset_experience_buffer();

        if (cancel_requested)
            break;

        while (!all_done())
        {
            m_scene->get_agents_states(states);
            choose_actions(states, actions);
            m_scene->set_actions_get_rewards(actions, rewards);
            m_scene->get_agents_states(next_states);
            m_scene->get_agents_done_status(dones);

            for (int i = 0; i < agents_count; i++)
            {
                total_rewards[i][k] += rewards[i];
            }

            store_experience(states, actions, rewards, next_states, dones);
            train();

            if (cancel_requested)
                break;

            QThread::msleep(16);
        }

        for (int i = 0; i < agents_count; i++)
        {
            episode_rewards[i] = total_rewards[i][k];
        }

        emit update_logs(episode_rewards, k);

        if (epsilon_t < 0.1)
        {
            epsilon_t = 0.1;
        }
        else
        {
            epsilon_t *= 0.99;
        }


        set_epsilon(epsilon_t);
    }

    emit training_finished(total_rewards, agents_coords);
    QMetaObject::invokeMethod(m_scene, "reset_env", Qt::BlockingQueuedConnection);
    free_qlearn();

    emit scene_disabled(false);
}
