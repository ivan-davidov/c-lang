#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H

#include "fsm.h"

enum traffic_light_state { GREEN, YELLOW, RED };
enum traffic_light_event { TIMER_TICK, PED_BUTTON };

Fsm *traffic_light_create();
void traffic_light_destroy(Fsm *fsm);

#endif
