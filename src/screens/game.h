#ifndef GAME_H
#define GAME_H

#include <time.h>

#include "../board.h"
#include "../screen.h"
#include "../snake.h"
#include "../utils/game.h"

#define HIT_PENALTY 10
#define HIT_BONUS 5

int game_screen(Screen screen, struct timespec *loop_delay,
                MemBase elements_memory, MemBase screen_memory, int *winner,
                int *snake1_score, int *snake2_score);

#endif