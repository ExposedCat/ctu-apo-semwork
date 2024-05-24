#ifndef BOARD_H
#define BOARD_H

#include <stdio.h>

#include "screen.h"

typedef enum KnobPressed {
    RED_KNOB = 4,
    GREEN_KNOB = 2,
    BLUE_KNOB = 1
} KnobPressed;

int init_board(Screen *screen, MemBase *screen_memory, MemBase *knobs_memory);

void get_knobs_state(MemBase knobs_memory, int *red_knob, int *green_knob,
                     int *blue_knob, int *clicked);

#endif