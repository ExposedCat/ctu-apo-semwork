#ifndef SNAKE_H
#define SNAKE_H

#include <stdlib.h>

#include "screen.h"

#define Snake Snake_t*

typedef enum Direction { UP, DOWN, LEFT, RIGHT } Direction;

typedef struct Snake_t {
    int length;
    Color color;
    int x;
    int y;
    Direction direction;
} Snake_t;

Snake create_snake(int length, int color, int x, int y, Direction direction);

void render_snake(Screen screen, Snake snake);

void move_snake(Screen screen, Snake snake);

#endif