#ifndef FIS_POST_PROC_H_INCLUDED
#define FIS_POST_PROC_H_INCLUDED

#include <stdbool.h>

int discretise_fis_output(float du);

void pump_state_update(bool p1_is_duty, int *p1_state, int *p2_state, int du);

void pump_set_duty(bool *p1_is_duty, int p1_state, int p2_state, int p1_prev_state, int p2_prev_state);

#endif

