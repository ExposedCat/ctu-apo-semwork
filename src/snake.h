#ifndef SNAKE_H
#define SNAKE_H

#include "screen.h"

#define Snake Snake_t*

typedef enum Direction { UP, DOWN, LEFT, RIGHT } Direction;

typedef struct Snake_t {
    int length;
    Color color;
    int x;
    int y;
} Snake_t;

Snake create_snake(int length, int color, int x, int y);

void render_snake(Screen screen, Snake snake);

#endif