
#include "game.h"

void set_speed(struct timespec *loop_delay, MemBase elements_memory,
               int value) {
    loop_delay->tv_nsec = (256 - value) * 1000 * 1000;
    set_led_line(elements_memory, value / 8);
}
