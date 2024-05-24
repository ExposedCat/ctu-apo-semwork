#ifndef BOARD_H
#define BOARD_H

#include <stdio.h>
#include <unistd.h>

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "screen.h"

#define MemBase unsigned char *

typedef enum KnobPressed {
    RED_KNOB = 4,
    GREEN_KNOB = 2,
    BLUE_KNOB = 1
} KnobPressed;

int init_board(Screen *screen, MemBase *screen_memory, MemBase *knobs_memory);

void get_knobs_state(MemBase knobs_memory, int *red_knob, int *green_knob,
                     int *blue_knob, int *clicked);

void set_led_line(MemBase board_memory, int units);

#endif