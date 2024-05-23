#include "snake.h"

#define TILE_WIDTH 20

Snake create_snake(int length, int color, int x, int y) {
    Snake snake = malloc(sizeof(Snake));
    snake->length = length;
    snake->color = color;
    snake->x = x;
    snake->y = y;
    return snake;
}

void render_snake(Screen screen, Snake snake) {
    draw_scaled_pixel(screen, snake->x, snake->y, TILE_WIDTH, snake->color);
}