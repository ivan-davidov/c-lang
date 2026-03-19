#ifndef FSM_H
#define FSM_H

#define MAX_ACTIONS 10

typedef void (*Func_ptr)(void);

typedef struct Fsm Fsm;
typedef struct Fsm_transition {
    int next_state;
    Func_ptr func_ptrs[MAX_ACTIONS];
    int num_actions;
} Fsm_transition;

Fsm *fsm_create(const Fsm_transition *fsm_transition, const char **states_names,
                int num_states, int num_events, int initial_state);
void fsm_destroy(Fsm *fsm);
void fsm_state_transition(Fsm *fsm, int event);
int fsm_get_current_state(const Fsm *fsm);
void fsm_print_state(const Fsm *fsm);

#endif
