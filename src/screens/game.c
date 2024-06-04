#include "game.h"

int init_entities(Snake *snake1, Snake *snake2, Food *food) {
    *snake1 = create_snake(RED, 0, 0, DOWN);
    if (!*snake1) {
        fprintf(stderr, "[CRITICAL]: Failed to create snake 1\n");
        return 1;
    }
    *snake2 = create_snake(BLUE, SCREEN_WIDTH - TILE_SIZE,
                           PLAYGROUND_HEIGHT - TILE_SIZE, UP);
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

    while (is_food_on_snake(*snake1, *food) ||
           is_food_on_snake(*snake2, *food)) {
        move_food(*food);
    }

    return 0;
}

void render_score_panel(Screen screen, int score1, int score2) {
    for (int x = 0; x < SCREEN_WIDTH; ++x) {
        for (int y = SCREEN_HEIGHT - PANEL_HEIGHT; y < SCREEN_HEIGHT; ++y) {
            draw_pixel(screen, x, y, WHITE);
        }
    }
    char score_label[50];
    sprintf(score_label, "Red %d", score1);
    draw_string(screen, 15, SCREEN_HEIGHT - PANEL_HEIGHT + 5, score_label,
                BLACK);

    sprintf(score_label, "Blue %d", score2);
    draw_string(screen, SCREEN_WIDTH - 100, SCREEN_HEIGHT - PANEL_HEIGHT + 5,
                score_label, BLACK);

    draw_string(screen, CENTER_X, SCREEN_HEIGHT - PANEL_HEIGHT + 5,
                score1 > score2 ? "RED" : (score2 > score1 ? "BLUE" : "draw"),
                BLACK);
}

// winner is 1 = Red player, 2 = Blue player, 0 = draw, -1 = game is not over
int game_screen(Screen screen, struct timespec *loop_delay,
                MemBase elements_memory, MemBase screen_memory, int *winner,
                int *snake1_score, int *snake2_score) {
    *winner = -1;

    Snake snake1 = NULL;
    Snake snake2 = NULL;
    Food food = NULL;
    int init_entities_exit = init_entities(&snake1, &snake2, &food);
    if (init_entities_exit) {
        return init_entities_exit;
    }

    int previous_red_knob = 0;
    int previous_blue_knob = 0;
    get_knobs_state(elements_memory, &previous_red_knob, NULL,
                    &previous_blue_knob, NULL);

    while (1) {
        int pressed = 0;
        int red_knob = 0;
        int blue_knob = 0;
        int green_knob = 0;
        get_knobs_state(elements_memory, &red_knob, &green_knob, &blue_knob,
                        &pressed);

        if (pressed == GREEN_KNOB) {
            *winner = snake1->length > snake2->length
                          ? 1
                          : (snake2->length > snake1->length ? 2 : 0);
            break;
        }

        set_speed(loop_delay, elements_memory, green_knob);

        int8_t red_knob_diff = red_knob - previous_red_knob;
        int8_t blue_knob_diff = blue_knob - previous_blue_knob;

        int red_knob_change = 0;
        int blue_knob_change = 0;

        if (red_knob_diff > 3) {
            red_knob_change = 1;
            previous_red_knob += 4;
        } else if (red_knob_diff < -3) {
            red_knob_change = -1;
            previous_red_knob -= 4;
        }

        if (blue_knob_diff > 3) {
            blue_knob_change = 1;
            previous_blue_knob += 4;
        } else if (blue_knob_diff < -3) {
            blue_knob_change = -1;
            previous_blue_knob -= 4;
        }

        reset_screen(screen);

        rotate_snake(snake1, red_knob_change);
        rotate_snake(snake2, blue_knob_change);

        move_snake(screen, snake1);
        move_snake(screen, snake2);

        int snake1_dead = ensure_snake_collisions(snake1, snake2, food);
        int snake2_dead = ensure_snake_collisions(snake2, snake1, food);

        if (snake1_dead || snake2_dead) {
            if (snake1_dead && snake2_dead) {
                *winner = 0;
            } else {
                if (snake1_dead) {
                    *snake1_score = snake1->length - HIT_PENALTY;
                    *snake2_score = snake2->length + HIT_BONUS;
                } else if (snake2_dead) {
                    *snake1_score = snake1->length + HIT_BONUS;
                    *snake2_score = snake2->length - HIT_PENALTY;
                }
                *winner = *snake1_score > *snake2_score ? 1 : 2;
            }
            break;
        }

        render_snake(screen, snake1);
        render_snake(screen, snake2);
        render_food(screen, food);

        render_score_panel(screen, snake1->length, snake2->length);

        parlcd_write_cmd(screen_memory, 0x2c);
        for (int pixel = 0; pixel < SCREEN_WIDTH * SCREEN_HEIGHT; pixel++) {
            parlcd_write_data(screen_memory, screen[pixel]);
        }

        clock_nanosleep(CLOCK_MONOTONIC, 0, loop_delay, NULL);
    }

    set_led_line(elements_memory, 0);

    parlcd_write_cmd(screen_memory, 0x2c);
    for (int pixel = 0; pixel < SCREEN_WIDTH * SCREEN_HEIGHT; pixel++) {
        parlcd_write_data(screen_memory, 0);
    }

    destroy_food(food);
    destroy_snake(snake1);
    destroy_snake(snake2);

    return 0;
}
