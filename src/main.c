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
    int i, j;
    int ptr;
    unsigned int c;
    screen = (Screen)malloc(320 * 480 * 2);
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
    ptr = 0;
    for (i = 0; i < 320; i++) {
        for (j = 0; j < 480; j++) {
            c = 0;
            screen[ptr] = c;
            parlcd_write_data(parlcd_mem_base, screen[ptr++]);
        }
    }
    font = &font_winFreeSystem14x16;

    // Init and lifecycle of the game
    draw_menu(screen, font, parlcd_mem_base);
    wait_for_start(mem_base);

    Snake snake1 = create_snake(1, YELLOW, 100, 100);
    Snake snake2 = create_snake(1, BLUE, 480 - 50, 320 - 50);

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
        xx = ((r & 0xff) * 480) / 256;  // blue
        // yy = (((r>>8)&0xff)*320)/256;//green
        zz = (((r >> 16) & 0xff) * 160) / 256;  // red

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

        for (ptr = 0; ptr < 320 * 480; ptr++) {
            screen[ptr] = 0u;
        }

        // update_snake_position(&snake1);
        // update_snake_position(&snake2);

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
        for (ptr = 0; ptr < 480 * 320; ptr++) {
            parlcd_write_data(parlcd_mem_base, screen[ptr]);
        }

        clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    }

    parlcd_write_cmd(parlcd_mem_base, 0x2c);
    for (ptr = 0; ptr < 480 * 320; ptr++) {
        parlcd_write_data(parlcd_mem_base, 0);
    }

    // led_line(mem_base, val_line);

    printf("[MAIN] Program finished\n");

    return 0;
}