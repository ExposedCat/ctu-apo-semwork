#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "../board.h"
#include "../screen.h"
#include "../snake.h"
#include "../utils/game.h"

#define HIT_PENALTY 10
#define HIT_BONUS 5

int main_menu_screen(Screen screen, struct timespec *loop_delay,
                     MemBase elements_memory, MemBase screen_memory);

#endif