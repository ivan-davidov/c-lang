#include "traffic_light.h"
#include "fsm.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_LIGHT 7

void turn_green_light_on(void);
void turn_green_light_off(void);
void turn_yellow_light_on(void);
void turn_yellow_light_off(void);
void turn_red_light_on(void);
void turn_red_light_off(void);
void start_ped_button_timer(void);
void reset_ped_button_timer(void);

static const char *state_names[] = {"GREEN", "YELLOW", "RED"};

static const Fsm_transition fsm_transition[][2] = {
    [GREEN][TIMER_TICK] = {.next_state = YELLOW,
                           .func_ptrs = {turn_green_light_off,
                                         turn_yellow_light_on},
                           .num_actions = 2},
    [GREEN][PED_BUTTON] = {.next_state = RED,
                           .func_ptrs = {start_ped_button_timer,
                                         turn_red_light_on,
                                         turn_green_light_off},
                           .num_actions = 3},
    [YELLOW][TIMER_TICK] = {.next_state = RED,
                            .func_ptrs = {turn_yellow_light_off,
                                          turn_red_light_on},
                            .num_actions = 2},
    [YELLOW][PED_BUTTON] = {.next_state = YELLOW,
                            .func_ptrs = {reset_ped_button_timer},
                            .num_actions = 1},
    [RED][TIMER_TICK] = {.next_state = GREEN,
                         .func_ptrs = {turn_red_light_off, turn_green_light_on},
                         .num_actions = 2},
    [RED][PED_BUTTON] = {.next_state = RED,
                         .func_ptrs = {reset_ped_button_timer},
                         .num_actions = 1}};

Fsm *traffic_light_create()
{
    Fsm *fsm = fsm_create((const Fsm_transition *)fsm_transition, state_names,
                          3, 2, GREEN);

    if (fsm != NULL) {
        printf("Created FSM: traffic_light\n");
    }
    return fsm;
}

void traffic_light_destroy(Fsm *fsm)
{
    if (fsm == NULL) {
        fprintf(stderr, "Nothing to destroy, FSM: traffic_light is NULL\n");
        return;
    }

    printf("Destroying FSM: traffic_light\n");
    fsm_destroy(fsm);
}

void turn_green_light_on(void) { printf("GREEN LIGHT ON\n"); }
void turn_green_light_off(void) { printf("GREEN LIGHT OFF\n"); }
void turn_yellow_light_on(void) { printf("YELLOW LIGHT ON\n"); }
void turn_yellow_light_off(void) { printf("YELLOW LIGHT OFF\n"); }
void turn_red_light_on(void) { printf("RED LIGHT ON\n"); }
void turn_red_light_off(void) { printf("RED LIGHT OFF\n"); }
void start_ped_button_timer(void) { printf("PED BUTTON TIMER ON\n"); }
void reset_ped_button_timer(void) { printf("PED BUTTON TIMER OFF\n"); }
