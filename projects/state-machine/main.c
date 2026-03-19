#include "debounce_button.h"
#include "fsm.h"
#include "traffic_light.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    // traffic light
    printf("TRAFFIC LIGHT FSM TEST\n");
    Fsm *fsm_traffic_light = traffic_light_create();
    fsm_print_state(fsm_traffic_light);

    fsm_state_transition(fsm_traffic_light, TIMER_TICK);
    fsm_print_state(fsm_traffic_light);
    fsm_state_transition(fsm_traffic_light, TIMER_TICK);
    fsm_print_state(fsm_traffic_light);
    fsm_state_transition(fsm_traffic_light, PED_BUTTON);
    fsm_print_state(fsm_traffic_light);
    fsm_state_transition(fsm_traffic_light, PED_BUTTON);
    fsm_print_state(fsm_traffic_light);
    fsm_state_transition(fsm_traffic_light, TIMER_TICK);
    fsm_print_state(fsm_traffic_light);

    traffic_light_destroy(fsm_traffic_light);

    // debounce button
    printf("DEBOUNCE BUTTON FSM TEST\n");
    Fsm *fsm_debounce_button = debounce_button_create();
    fsm_print_state(fsm_debounce_button);

    fsm_state_transition(fsm_debounce_button, BTN_PRESS);
    fsm_print_state(fsm_debounce_button);
    fsm_state_transition(fsm_debounce_button, BTN_PRESS);
    fsm_print_state(fsm_debounce_button);
    fsm_state_transition(fsm_debounce_button, BTN_RELEASE);
    fsm_print_state(fsm_debounce_button);
    fsm_state_transition(fsm_debounce_button, BTN_PRESS);
    fsm_print_state(fsm_debounce_button);
    fsm_state_transition(fsm_debounce_button, BTN_RELEASE);
    fsm_print_state(fsm_debounce_button);
    fsm_state_transition(fsm_debounce_button, BTN_PRESS);
    fsm_print_state(fsm_debounce_button);
    fsm_state_transition(fsm_debounce_button, BTN_RELEASE);
    fsm_print_state(fsm_debounce_button);
    fsm_state_transition(fsm_debounce_button, BTN_RELEASE);
    fsm_print_state(fsm_debounce_button);

    debounce_button_destroy(fsm_debounce_button);
    return EXIT_SUCCESS;
}
