
#include "food.h"

Food create_food(int x, int y, Color color, int weight) {
    Food food = (Food)malloc(sizeof(Food_t));
    if (food == NULL) {
        return NULL;
    }
    food->x = x;
    food->y = y;
    food->weight = weight;
    food->color = color;
    return food;
}

void render_food(Screen screen, Food food) {
    int offset = (TILE_SIZE - FOOD_SIZE) / 2;
    draw_scaled_pixel(screen, food->x + offset, food->y + offset, FOOD_SIZE,
                      food->color);
}

int collides_with_food(Food food, int x, int y) {
    return x == food->x && y == food->y;
}

void destroy_food(Food food) { free(food); }

void move_food(Food food) {
    int x, y;
    choose_random_tile(&x, &y);
    food->x = x;
    food->y = y;
}