#include "snake.h"

#define TILE_WIDTH 20

Snake create_snake(int length, int color, int x, int y, Direction direction) {
    Snake snake = (Snake)malloc(sizeof(Snake_t));
    if (snake == NULL) {
        return NULL;
    }
    snake->length = length;
    snake->color = color;
    snake->x = x;
    snake->y = y;
    snake->direction = direction;
    return snake;
}

void render_snake(Screen screen, Snake snake) {
    draw_scaled_pixel(screen, snake->x, snake->y, TILE_WIDTH, snake->color);
}

void rotate_snake(Snake snake, int rotation_change) {
    if (abs(rotation_change) < 10) {
        return;
    }
    if (rotation_change > 0 && rotation_change < 195 ||
        rotation_change <= -195) {
        snake->direction = (snake->direction - 1) % 4;
    } else if (rotation_change < 0 && rotation_change > -195 ||
               rotation_change >= 195) {
        snake->direction = (snake->direction + 1) % 4;
    }
}

void move_snake(Screen screen, Snake snake) {
    int change_x = snake->direction == LEFT    ? -1
                   : snake->direction == RIGHT ? 1
                                               : 0;
    int change_y = snake->direction == UP     ? -1
                   : snake->direction == DOWN ? 1
                                              : 0;
    snake->x += change_x * TILE_WIDTH;
    if (snake->x < 0) {
        snake->x += SCREEN_WIDTH;
    }
    if (snake->x > SCREEN_WIDTH) {
        snake->x -= SCREEN_WIDTH;
    }

    snake->y += change_y * TILE_WIDTH;
    if (snake->y < 0) {
        snake->y += SCREEN_HEIGHT;
    }
    if (snake->y > SCREEN_HEIGHT) {
        snake->y -= SCREEN_HEIGHT;
    }
}