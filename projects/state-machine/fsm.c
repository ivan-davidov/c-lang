#include "fsm.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct Fsm {
    const Fsm_transition *table;
    const char **states_names;
    int num_states;
    int num_events;
    int current_state;

} Fsm;

Fsm *fsm_create(const Fsm_transition *fsm_transition, const char **states_names,
                int num_states, int num_events, int initial_state)
{
    if (fsm_transition == NULL) {
        fprintf(stderr, "Invalid Fsm_transition object - NULL\n");
        return NULL;
    }

    if (states_names == NULL) {
        fprintf(stderr, "Must provide state names, given NULL\n");
        return NULL;
    }

    Fsm *fsm_ptr = malloc(sizeof(Fsm));
    if (fsm_ptr == NULL) {
        fprintf(stderr, "can't allocate for Fsm\n");
        return NULL;
    }

    fsm_ptr->table = fsm_transition;
    fsm_ptr->states_names = states_names;
    fsm_ptr->num_states = num_states;
    fsm_ptr->num_events = num_events;
    fsm_ptr->current_state = initial_state;

    return fsm_ptr;
}

void fsm_destroy(Fsm *fsm)
{
    if (fsm != NULL) {
        free(fsm);
    }
}

void fsm_state_transition(Fsm *fsm, int event)
{
    if (fsm == NULL) {
        fprintf(stderr, "Can transition on NULL FSM\n");
        return;
    }

    if (fsm->current_state < 0 || fsm->current_state >= fsm->num_states ||
        event < 0 || event >= fsm->num_events) {
        fprintf(stderr, "invalid argument\n");
        return;
    }

    Fsm_transition transition =
        fsm->table[fsm->current_state * fsm->num_events + event];

    fsm->current_state = transition.next_state;

    for (int i = 0; i < transition.num_actions; i++) {
        transition.func_ptrs[i]();
    }
}

int fsm_get_current_state(const Fsm *fsm)
{
    if (fsm == NULL) {
        fprintf(stderr, "Fsm is NULL\n");
        return -1;
    }

    return fsm->current_state;
}

void fsm_print_state(const Fsm *fsm)
{
    if (fsm == NULL) {
        fprintf(stderr, "Fsm is NULL\n");
        return;
    }

    printf("Current state of FSM is %s\n",
           fsm->states_names[fsm->current_state]);
}
