#ifndef GAME_OVER_H
#define GAME_OVER_H

#include "../board.h"
#include "../screen.h"
#include "../snake.h"

#define HIT_PENALTY 10
#define HIT_BONUS 5

void game_over_screen(Screen screen, struct timespec *loop_delay,
                      MemBase elements_memory, MemBase screen_memory,
                      int winner, int snake1_score, int snake2_score);

#endif