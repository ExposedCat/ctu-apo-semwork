#ifndef FOOD_H
#define FOOD_H

#include <stdlib.h>

#include "screen.h"

#define Food Food_t*

typedef struct Food_t {
    int x;
    int y;
    int weight;
    Color color;
} Food_t;

Food create_food(int x, int y, Color color, int weight);

void render_food(Screen screen, Food food);

int collides_with_food(Food food, int x, int y);

void destroy_food(Food food);

void move_food(Food food);

#endif