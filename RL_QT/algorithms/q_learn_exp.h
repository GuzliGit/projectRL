#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define BUFFER_SIZE 1000
#define BATCH_SIZE 75

typedef struct
{
    short state;
    char action;
    signed char reward;
    short next_state;
    char done;
} Exp_buffer;

typedef struct
{
    double **Q;
    short current_buf_size;
    Exp_buffer *buf;
} Q_Agent;

extern double alpha_p, gamma_p, epsilon_p;
extern short agents_count;
extern short state_size;
extern short max_actions;
extern Q_Agent *agents;

void init_qlearn(short state_size, short agents_count, double alpha, double gamma, double epsilon);

void set_q_table(int agent_id, double** q);

void choose_actions(short *states, char *actions);

void store_experience(short *states, char *actions, signed char *rewards, short *next_states, char *dones);

void train();

void free_qlearn();

char all_done();

void reset_experience_buffer();

void set_epsilon(double new_val);

double **get_q_table(short agent_id);

short get_current_buf_size(short agent_id);

char is_actions_limit(int agent_id);

#ifdef __cplusplus
}
#endif
