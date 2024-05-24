/*******************************************************************
 Multiplayer Snake Game
 by Ievgeniia Rachkovska and Artem Prokop


 *******************************************************************/

#define _POSIX_C_SOURCE 200112L

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "board.h"
#include "font_types.h"
#include "food.h"
#include "screens.h"
#include "snake.h"

int main(int argc, char *argv[]) {
    printf("[MAIN] Program started\n");

    Screen screen = NULL;
    MemBase screen_memory = NULL;
    MemBase elements_memory = NULL;

    int board_init_exit = init_board(&screen, &screen_memory, &elements_memory);
    if (board_init_exit) {
        return board_init_exit;
    }

    struct timespec loop_delay;
    loop_delay.tv_sec = 0;

    while (1) {
        int main_menu_exit = main_menu_screen(screen, &loop_delay,
                                              elements_memory, screen_memory);
        if (main_menu_exit) {
            if (main_menu_exit == 42) {
                break;
            }
            return main_menu_exit;
        }

        int winner = -1;
        int snake1_score = 0;
        int snake2_score = 0;
        int game_exit =
            game_screen(screen, &loop_delay, elements_memory, screen_memory,
                        &winner, &snake1_score, &snake2_score);
        if (game_exit) {
            return game_exit;
        }

        game_over_screen(screen, &loop_delay, elements_memory, screen_memory,
                         winner, snake1_score, snake2_score);
    }

    free(screen);

    printf("[MAIN] Program finished\n");

    return 0;
}