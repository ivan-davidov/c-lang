#ifndef DEBOUNCE_BUTTON_H
#define DEBOUNCE_BUTTON_H

#include "fsm.h"

enum debounce_button_state { PRESSED, UNPRESSED };
enum debounce_button_event { BTN_PRESS, BTN_RELEASE };

Fsm *debounce_button_create();
void debounce_button_destroy(Fsm *rsm);

#endif
