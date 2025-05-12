#include "q_learn_exp.h"
#include <stdlib.h>
#include <time.h>

double alpha_p, gamma_p, epsilon_p;
short agents_count;
short state_size;
short max_actions = 4;
Q_Agent *agents = NULL;
Exp_buffer *shared_buffer = NULL;

void init_qlearn(short state_size_t, short agents_count_t, double alpha_t, double gamma_t, double epsilon_t)
{
    srand(time(NULL));

    state_size = state_size_t;
    alpha_p = alpha_t;
    gamma_p = gamma_t;
    epsilon_p = epsilon_t;
    agents_count = agents_count_t;

    shared_buffer = malloc(BUFFER_SIZE * agents_count * sizeof(Exp_buffer));
    agents = malloc(agents_count * sizeof(Q_Agent));
    for (int i = 0; i < agents_count; i++)
    {
        agents[i].Q = malloc(state_size * sizeof(double*));
        for (int j = 0; j < state_size; j++)
        {
            agents[i].Q[j] = calloc(max_actions, sizeof(double));
        }

        agents[i].current_buf_size = 0;
        agents[i].actions_count = 0;
        agents[i].buf = shared_buffer + BUFFER_SIZE * i;
    }
}

void choose_actions(short *states, char *actions)
{
    for (int i = 0; i < agents_count; i++)
    {
        if ((double)rand() / RAND_MAX < epsilon_p)
        {
            actions[i] = rand() % max_actions;
            agents[i].actions_count++;
            continue;
        }

        char best = 0;
        for (int j = 1; j < max_actions; j++)
        {
            if (agents[i].Q[states[i]][j] > agents[i].Q[states[i]][best])
            {
                best = j;
            }
        }

        actions[i] = best;
        agents[i].actions_count++;
    }
}

void store_experience(short *states, char *actions, signed char *rewards, short *next_states, char *dones)
{
    for (int i = 0; i < agents_count; i++)
    {
        short buf_id = agents[i].current_buf_size % BUFFER_SIZE;
        agents[i].buf[buf_id] = (Exp_buffer){states[i], actions[i], rewards[i], next_states[i], dones[i]};
        agents[i].current_buf_size++;
    }
}

void train()
{
    for(int i = 0; i < agents_count; i++)
    {
        short buffer_size = (agents[i].current_buf_size >= BUFFER_SIZE) ? BUFFER_SIZE : agents[i].current_buf_size;

        if (buffer_size < BATCH_SIZE)
        {
            continue;
        }

        for (int j = 0; j < BATCH_SIZE; j++)
        {
            short id = rand() % buffer_size;
            Exp_buffer exp = agents[i].buf[id];

            double max_q_next = 0.0;
            if (!exp.done)
            {
                max_q_next = agents[i].Q[exp.next_state][0];
                for (int j = 1; j < max_actions; j++)
                {
                    if (agents[i].Q[exp.next_state][j] > max_q_next)
                    {
                        max_q_next = agents[i].Q[exp.next_state][j];
                    }
                }
            }

            agents[i].Q[exp.state][exp.action] += alpha_p * (exp.reward + gamma_p * max_q_next - agents[i].Q[exp.state][exp.action]);
        }
    }
}

void free_qlearn() {
    for (int i = 0; i < agents_count; i++)
    {
        for (int j = 0; j < state_size; j++)
        {
            free(agents[i].Q[j]);
        }

        free(agents[i].Q);
    }

    free(shared_buffer);
    free(agents);
}

char all_done()
{
    short done_count = 0;
    short agents_on_limit = 0;
    for (int i = 0; i < agents_count; i++)
    {
        short last_index = (agents[i].current_buf_size - 1) % BUFFER_SIZE;
        if (agents[i].buf[last_index].done && agents[i].actions_count > 0)
            done_count++;

        if (is_actions_limit(i))
            agents_on_limit++;
    }

    if (done_count == agents_count || agents_on_limit == agents_count)
    {
        for (int i = 0; i < agents_count; i++)
        {
            agents[i].actions_count = 0;
        }
        return 1;
    }
    else
        return 0;
}

void set_epsilon(double new_val)
{
    epsilon_p = new_val;
}

double **get_q_table(short agent_id)
{
    if (agent_id >= agents_count || agent_id < 0)
        return NULL;

    return agents[agent_id].Q;
}

unsigned int get_current_buf_size(short agent_id)
{
    if (agent_id >= agents_count || agent_id < 0)
        return -1;

    return agents[agent_id].current_buf_size > BUFFER_SIZE ? BUFFER_SIZE : agents[agent_id].current_buf_size;
}

char is_actions_limit(int agent_id)
{
    if (agents[agent_id].actions_count >= ACTIONS_LIMIT)
        return 1;

    return 0;
}

void set_q_table(int agent_id, double **q)
{
    for (int i = 0; i < state_size; i++)
    {
        for (int j = 0; j < max_actions; j++)
        {
            agents[agent_id].Q[i][j] = q[i][j];
        }
    }
}
