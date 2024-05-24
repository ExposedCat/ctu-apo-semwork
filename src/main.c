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
#include "src/food.h"
#include "src/screen.h"
#include "src/snake.h"
#include "src/utils.h"

void reset_screen(Screen screen) {
    for (int pixel = 0; pixel < SCREEN_HEIGHT * SCREEN_WIDTH; pixel++) {
        screen[pixel] = 0u;
    }
}

typedef enum KnobPressed {
    RED_KNOB = 4,
    GREEN_KNOB = 2,
    BLUE_KNOB = 1
} KnobPressed;

void get_knobs_state(MemBase knobs_memory, int *red_knob, int *green_knob,
                     int *blue_knob, int *clicked) {
    int rgb_knobs_value =
        *(volatile uint32_t *)(knobs_memory + SPILED_REG_KNOBS_8BIT_o);

    if (blue_knob) {
        *blue_knob = (rgb_knobs_value & 0x000000ff);
    }
    if (red_knob) {
        *red_knob = (rgb_knobs_value & 0x00ff0000) >> 16;
    }
    if (green_knob) {
        *green_knob = (rgb_knobs_value & 0x0000ff00) >> 8;
    }
    if (clicked) {
        *clicked = (rgb_knobs_value & 0xff000000) >> 24;
    }
}

int init_board(Screen *screen, MemBase *screen_memory, MemBase *knobs_memory) {
    *screen = (Screen)malloc(SCREEN_HEIGHT * SCREEN_WIDTH * 2);

    *screen_memory = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
    if (!*screen_memory) {
        fprintf(stderr,
                "Failed to initialize board: Screen memory allocation "
                "failed\n");
        return 1;
    }

    *knobs_memory = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
    if (!*knobs_memory) {
        fprintf(stderr,
                "Failed to initialize board: Knobs memory allocation "
                "failed\n");
        return 1;
    }

    parlcd_hx8357_init(*screen_memory);

    parlcd_write_cmd(*screen_memory, 0x2c);
    for (int pixel = 0; pixel < SCREEN_HEIGHT * SCREEN_WIDTH; ++pixel) {
        (*screen)[pixel] = 0;
        parlcd_write_data(*screen_memory, (*screen)[pixel]);
    }

    return 0;
}

int init_entities(Snake *snake1, Snake *snake2, Food *food) {
    *snake1 = create_snake(YELLOW, 0, 0, DOWN);
    if (!*snake1) {
        fprintf(stderr, "[CRITICAL]: Failed to create snake 1\n");
        return 1;
    }
    *snake2 = create_snake(BLUE, SCREEN_WIDTH - TILE_SIZE,
                           SCREEN_HEIGHT - TILE_SIZE, UP);
    if (!*snake2) {
        destroy_snake(*snake1);
        fprintf(stderr, "[CRITICAL]: Failed to create snake 2\n");
        return 1;
    }

    *food = create_food(0, 0, WHITE, 1);
    if (!*food) {
        destroy_snake(*snake1);
        destroy_snake(*snake2);
        fprintf(stderr, "[CRITICAL]: Failed to create food\n");
        return 1;
    }
    while (collides_with_food(*food, (*snake1)->head->x, (*snake1)->head->y) ||
           collides_with_food(*food, (*snake2)->head->x, (*snake2)->head->y)) {
        move_food(*food);
    }

    return 0;
}

void draw_menu(Screen screen, MemBase screen_memory, int red_held,
               int blue_held) {
    clear_screen(screen);

    draw_string(screen, 100, 100, "Multiplayer Snake Game", BLACK);
    draw_string(screen, 100, 120, "by Ievgeniia Rachkovska and Artem Prokop",
                BLACK);

    draw_string(screen, 10, SCREEN_HEIGHT - 50, "Player 1", BLACK);
    draw_string(screen, 10, SCREEN_HEIGHT - 20,
                red_held ? "Ready!" : "Ready? Hold RED knob", BLACK);
    draw_string(screen, SCREEN_WIDTH - 90, SCREEN_HEIGHT - 50, "Player 2",
                BLACK);
    draw_string(screen, SCREEN_WIDTH - (blue_held ? 80 : 180),
                SCREEN_HEIGHT - 20,
                blue_held ? "Ready!" : "Ready? Hold BLUE knob", BLACK);

    parlcd_write_cmd(screen_memory, 0x2c);
    for (int pixel = 0; pixel < SCREEN_WIDTH * SCREEN_HEIGHT; ++pixel) {
        parlcd_write_data(screen_memory, screen[pixel]);
    }
}

int main(int argc, char *argv[]) {
    printf("[MAIN] Program started\n");

    Screen screen = NULL;
    MemBase screen_memory = NULL;
    MemBase knobs_memory = NULL;

    int board_init_exit = init_board(&screen, &screen_memory, &knobs_memory);
    if (board_init_exit) {
        return board_init_exit;
    }

    int pressed = 0;
    while (pressed != RED_KNOB + BLUE_KNOB) {
        get_knobs_state(knobs_memory, NULL, NULL, NULL, &pressed);
        draw_menu(screen, screen_memory, pressed == RED_KNOB,
                  pressed == BLUE_KNOB);
    }

    Snake snake1 = NULL;
    Snake snake2 = NULL;
    Food food = NULL;
    int init_entities_exit = init_entities(&snake1, &snake2, &food);
    if (init_entities_exit) {
        return init_entities_exit;
    }

    struct timespec loop_delay;
    loop_delay.tv_sec = 0;
    loop_delay.tv_nsec = 250 * 1000 * 1000;

    int previous_red_knob = 0;
    int previous_blue_knob = 0;

    while (1) {
        int red_knob = 0;
        int blue_knob = 0;
        get_knobs_state(knobs_memory, &red_knob, NULL, &blue_knob, &pressed);

        if (pressed == GREEN_KNOB) {
            break;
        }

        int red_knob_change = red_knob - previous_red_knob;
        int blue_knob_change = blue_knob - previous_blue_knob;

        previous_blue_knob = blue_knob;
        previous_red_knob = red_knob;

        reset_screen(screen);

        rotate_snake(snake1, red_knob_change);
        rotate_snake(snake2, blue_knob_change);

        move_snake(screen, snake1);
        move_snake(screen, snake2);

        int snake_dead = ensure_snake_collisions(snake1, snake2, food);
        snake_dead = ensure_snake_collisions(snake2, snake1, food);

        render_snake(screen, snake1);
        render_snake(screen, snake2);
        render_food(screen, food);

        parlcd_write_cmd(screen_memory, 0x2c);
        for (int pixel = 0; pixel < SCREEN_WIDTH * SCREEN_HEIGHT; pixel++) {
            parlcd_write_data(screen_memory, screen[pixel]);
        }

        clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    }

    parlcd_write_cmd(screen_memory, 0x2c);
    for (int pixel = 0; pixel < SCREEN_WIDTH * SCREEN_HEIGHT; pixel++) {
        parlcd_write_data(screen_memory, 0);
    }

    destroy_food(food);
    destroy_snake(snake1);
    destroy_snake(snake2);

    printf("[MAIN] Program finished\n");

    return 0;
}