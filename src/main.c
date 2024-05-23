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

#include "font_types.h"
#include "mzapo_parlcd.h"
#include "src/screen.h"
#include "src/snake.h"
#include "src/utils.h"

Screen screen;
font_descriptor_t *font;

int main(int argc, char *argv[]) {
    unsigned char *parlcd_mem_base, *mem_base;
    screen = (Screen)malloc(SCREEN_HEIGHT * SCREEN_WIDTH * 2);
    uint32_t val_line = 3;

    printf("[MAIN] Program started\n");

    // some things so programe is working
    // I don`t know what is that
    parlcd_mem_base =
        map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
    if (parlcd_mem_base == NULL)
        exit(1);

    mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
    if (mem_base == NULL)
        exit(1);

    parlcd_hx8357_init(parlcd_mem_base);

    parlcd_write_cmd(parlcd_mem_base, 0x2c);
    int pixel = 0;
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        for (int j = 0; j < SCREEN_WIDTH; j++) {
            screen[pixel] = 0;
            parlcd_write_data(parlcd_mem_base, screen[pixel++]);
        }
    }
    font = &font_winFreeSystem14x16;

    // Init and lifecycle of the game
    draw_menu(screen, font, parlcd_mem_base);
    wait_for_start(mem_base);

    Snake snake1 = create_snake(1, YELLOW, 0, 0, DOWN);
    if (!snake1) {
        fprintf(stderr, "[MAIN][CRITICAL]: Failed to create snake 1\n");
        return 1;
    }
    printf("[MAIN]: Snake 1 created\n");
    Snake snake2 = create_snake(1, BLUE, SCREEN_WIDTH - TILE_WIDTH,
                                SCREEN_HEIGHT - TILE_WIDTH, UP);
    if (!snake2) {
        fprintf(stderr, "[MAIN][CRITICAL]: Failed to create snake 2\n");
        return 1;
    }
    printf("[MAIN]: Snake 2 created\n");

    //       Food_t food;
    // initialize_food(&food, 200, 150, RED);

    struct timespec loop_delay;
    loop_delay.tv_sec = 0;
    loop_delay.tv_nsec = 250 * 1000 * 1000;
    int xx = 0, yy = 0, zz = 0;
    int oldxx = 0, oldzz = 0;
    while (1) {
        int r = *(volatile uint32_t *)(mem_base + SPILED_REG_KNOBS_8BIT_o);
        if ((r & 0x7000000) != 0) {
            break;
        }
        xx = ((r & 0xff) * SCREEN_WIDTH) / 256;          // blue
        yy = (((r >> 8) & 0xff) * SCREEN_HEIGHT) / 256;  // green
        zz = (((r >> 16) & 0xff) * 160) / 256;           // red

        int delta1 = zz - oldzz;
        int delta2 = xx - oldxx;
        // change_direction(&snake1, delta1);
        // change_direction(&snake2, delta2);

        // if (snake1->length > 1) {
        //     update_snake_segments(&snake1);
        // }
        // if (snake2->length > 1) {
        //     update_snake_segments(&snake2);
        // }

        oldxx = xx;
        oldzz = zz;

        for (pixel = 0; pixel < SCREEN_HEIGHT * SCREEN_WIDTH; pixel++) {
            screen[pixel] = 0u;
        }

        printf("%d;%d;%d\n", xx, yy, zz);

        // update_snake_position(&snake1);
        // update_snake_position(&snake2);
        move_snake(screen, snake1);
        move_snake(screen, snake2);

        render_snake(screen, snake1);
        render_snake(screen, snake2);
        // render_snake(screen, snake3);
        // draw_snake(&snake1, 0xFF00);
        // draw_snake(&snake2, 0x001F);
        // draw_food(&food, 0x07E0);  // Green food

        // draw_score(snake1.snake_size, 10, 10, 0xFFFF);  // White color
        // printf("snake1 score %d\n", snake1.snake_size);

        // if (check_collision(&snake1, &food)) {
        //     printf("eat\n");
        //     grow_snake(&snake1);
        //     relocate_food(&food);
        // }

        parlcd_write_cmd(parlcd_mem_base, 0x2c);
        for (int pixel = 0; pixel < SCREEN_WIDTH * SCREEN_HEIGHT; pixel++) {
            parlcd_write_data(parlcd_mem_base, screen[pixel]);
        }

        clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    }

    parlcd_write_cmd(parlcd_mem_base, 0x2c);
    for (int pixel = 0; pixel < SCREEN_WIDTH * SCREEN_HEIGHT; pixel++) {
        parlcd_write_data(parlcd_mem_base, 0);
    }

    // led_line(mem_base, val_line);

    free(snake1);
    free(snake2);

    printf("[MAIN] Program finished\n");

    return 0;
}