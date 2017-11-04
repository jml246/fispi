/** @file fis_post_proc.c */

#include <stdbool.h>

/**
 * @brief Discretise the fuzzy controller output to integer values
 *
 * @param du fuzzy controller output (float)
 * @return fuzzy controller output discretised to give integer value in range [-2,2]
 */
int discretise_fis_output(float du)
{
    int out;
    if (du > 1.5) {
        out = 2;
    } else if (0.5 < du && du <= 1.5) {
        out = 1;
    } else if (-0.5 < du && du <= 0.5) {
        out = 0;
    } else if (-1.5 < du && du <= -0.5) {
        out = -1;
    } else if (-2 < du && du <=-1.5) {
        out = -2;
    }
    /// \todo What happens if du < 2 ?
    return out;
}

/**
 * @brief Determine new pump statuses using FIS output
 *
 * Update the pump state based on the current state of the two pumps,
 * which of them are duty and assist and the output of the fuzzy controller
 *
 * Pump states: 1 is on, 0 is off
 * Boolean values: 1 is true, 0 is false
 *
 * @param p1_is_duty Boolean value which specifies whether pump 1 or pump 2 is duty (the other will be the assist pump)
 * @param p1_state   The state of pump 1 prior to the function being called (new state output by function using pass-by-ref)
 * @param p2_state   See above
 * @param du         Discretised FIS output (value in range [-2,2])
 */
void pump_state_update(bool p1_is_duty, int *p1_state, int *p2_state, int du)
{
    if (p1_is_duty == true) {
        if (*p1_state==0 && *p2_state==0) {
            if (du==1) {
                *p1_state=1;
                *p2_state=0;
            } else if (du==2) {
                *p1_state=1;
                *p2_state=1;
            } else {
                *p1_state=0;
                *p2_state=0;
            }
        } else if (*p1_state==1 && *p2_state==0) {
            if (du==-1 || du==-2) {
                *p1_state=0;
                *p2_state=0;
            } else if (du==2 || du==1) {
                *p1_state=1;
                *p2_state=1;
            } else {
                *p1_state=1;
                *p2_state=0;
            }
        } else if (*p1_state==1 && *p2_state==1) {
            if (du==-1) {
                *p1_state=1;
                *p2_state=0;
            } else if (du==-2) {
                *p1_state=0;
                *p2_state=0;
            } else {
                *p1_state=1;
                *p2_state=1;
            }
        }
    } else if (p1_is_duty == false) {
        if (*p1_state==0 && *p2_state==0) {
            if (du==1) {
                *p2_state=1;
                *p1_state=0;
            } else if (du==2) {
                *p1_state=1;
                *p2_state=1;
            } else {
                *p1_state=0;
                *p2_state=0;
            }
        } else if (*p1_state==0 && *p2_state==1) {
            if (du==-1 || du==-2) {
                *p1_state=0;
                *p2_state=0;
            } else if (du==2 || du==1) {
                *p1_state=1;
                *p2_state=1;
            } else {
                *p1_state=0;
                *p2_state=1;
            }
        } else if (*p1_state==1 && *p2_state==1) {
            if (du==-1) {
                *p1_state=0;
                *p2_state=1;
            } else if (du==-2) {
                *p1_state=0;
                *p2_state=0;
            } else {
                *p1_state=1;
                *p2_state=1;
            }
        }
    }
}

/**
 * @brief Determine which of two inactive pumps is duty and which is assist
 *
 * If both pumps are set to off for the current timestep detemine which is duty and which
 * is assist based upon the state of both pumps from the _previous_ timestep.
 * 
 * @param p1_is_duty whether pump 1 is the duty pump and pump 2 is the assist or vice versa (updated by pass-by-ref) (boolean)
 * @param p1_state current state of pump 1 (integer in {0,1})
 * @param p2_state current state of pump 2 (integer in {0,1})
 * @param p1_prev_state previous state of pump 1 (integer in {0,1})
 * @param p2_prev_state previous state of pump 1 (integer in {0,1})
 */
void pump_set_duty(bool *p1_is_duty, int p1_state, int p2_state, int p1_prev_state, int p2_prev_state)
{
    if (p1_state==0 && p2_state==0) {
        if (p1_prev_state==1 && p2_prev_state==0) {
            *p1_is_duty = false;
        } else if (p1_prev_state==0 && p2_prev_state==1) {
            *p1_is_duty = true;
        } else if (p1_prev_state==1 && p2_prev_state==1 && *p1_is_duty == true) {
            *p1_is_duty = false;
        } else if (p1_prev_state==1 && p2_prev_state==1 && *p1_is_duty == false) {
            *p1_is_duty = true;
        }
    }
}
