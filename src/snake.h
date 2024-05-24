#ifndef SNAKE_H
#define SNAKE_H

#include <stdio.h>
#include <stdlib.h>

#include "food.h"
#include "screen.h"
#include "tile.h"

#define Snake Snake_t*

typedef enum Direction { UP, RIGHT, DOWN, LEFT } Direction;

typedef struct Snake_t {
    int length;
    Color color;
    Tile head;
    Direction direction;
    int got_bigger;
} Snake_t;

Snake create_snake(Color color, int x, int y, Direction direction);

void render_snake(Screen screen, Snake snake);

void move_snake(Screen screen, Snake snake);

void rotate_snake(Snake snake, int rotation_change);

void destroy_snake(Snake snake);

int is_food_on_snake(Snake snake, Food food);

// Returns true if snake is dead
int ensure_snake_collisions(Snake snake, Snake another_snake, Food food);

#endif