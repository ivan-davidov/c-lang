#include "debounce_button.h"
#include "fsm.h"
#include <stdio.h>

void press_the_button(void);
void release_the_button(void);
void noop(void);

static const char *state_names[] = {"PRESSED", "UNPRESSED"};

static const Fsm_transition fsm_transition[][2] = {
    [UNPRESSED][BTN_PRESS] = {.next_state = PRESSED,
                              .func_ptrs = {press_the_button},
                              .num_actions = 1},
    [UNPRESSED][BTN_RELEASE] = {.next_state = UNPRESSED,
                                .func_ptrs = {noop},
                                .num_actions = 1},
    [PRESSED][BTN_PRESS] = {.next_state = PRESSED,
                            .func_ptrs = {noop},
                            .num_actions = 1},
    [PRESSED][BTN_RELEASE] = {.next_state = UNPRESSED,
                              .func_ptrs = {release_the_button},
                              .num_actions = 1}};

Fsm *debounce_button_create()
{
    Fsm *fsm = fsm_create((const Fsm_transition *)fsm_transition, state_names,
                          2, 2, UNPRESSED);

    if (fsm != NULL) {
        printf("Created FSM: debounce_button\n");
    }
    return fsm;
}

void debounce_button_destroy(Fsm *fsm)
{
    if (fsm == NULL) {
        fprintf(stderr, "Nothing to destroy, FSM: debounce_button in NULL\n");
        return;
    }
    printf("Destroying FSM: debounce_button\n");
    fsm_destroy(fsm);
}

void press_the_button(void) { printf("BUTTON PRESSED\n"); }
void release_the_button(void) { printf("BUTTON RELEASED\n"); }
void noop(void) { printf("NOOP: DOING NOTHING\n"); }
