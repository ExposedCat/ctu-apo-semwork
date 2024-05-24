#include "snake.h"

#include "food.h"

Snake create_snake(Color color, int x, int y, Direction direction) {
    Snake snake = (Snake)malloc(sizeof(Snake_t));
    if (snake == NULL) {
        return NULL;
    }

    Tile head = create_tile(x, y, TILE_SIZE, color);
    if (head == NULL) {
        free(snake);
        return NULL;
    }

    snake->length = 1;
    snake->color = color;
    snake->head = head;
    snake->direction = direction;
    snake->got_bigger = 0;
    return snake;
}

void render_snake(Screen screen, Snake snake) {
    Tile tile = snake->head;
    do {
        render_tile(screen, tile);
        tile = tile->next;
    } while (tile != NULL);
}

int collides_with_snake(Snake snake, int x, int y) {
    Tile tile = snake->head;
    do {
        if (tile->x == x && tile->y == y) {
            return 1;
        }
        tile = tile->next;
    } while (tile != NULL);
    return 0;
}

int is_food_on_snake(Snake snake, Food food) {
    return collides_with_snake(snake, food->x, food->y);
}

int ensure_snake_collisions(Snake snake, Snake another_snake, Food food) {
    if (collides_with_food(food, snake->head->x, snake->head->y)) {
        snake->length += 1;
        snake->got_bigger = 1;
        while (is_food_on_snake(snake, food) ||
               is_food_on_snake(another_snake, food)) {
            move_food(food);
        }
    }

    if (collides_with_snake(another_snake, snake->head->x, snake->head->y)) {
        return 1;
    }

    return 0;
}

void rotate_snake(Snake snake, int rotation_change) {
    if (abs(rotation_change) < 10) {
        return;
    }
    if ((rotation_change > 0 && rotation_change < 195) ||
        rotation_change <= -195) {
        snake->direction = (snake->direction - 1) % 4;
    } else if ((rotation_change < 0 && rotation_change > -195) ||
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

    int new_x = snake->head->x + change_x * TILE_SIZE;
    int new_y = snake->head->y + change_y * TILE_SIZE;

    new_x += (new_x < 0 ? SCREEN_WIDTH
                        : (new_x >= SCREEN_WIDTH ? -SCREEN_WIDTH : 0));

    new_y +=
        (new_y < 0 ? PLAYGROUND_HEIGHT
                   : (new_y >= PLAYGROUND_HEIGHT ? -PLAYGROUND_HEIGHT : 0));

    if (!snake->got_bigger) {
        Tile parent = NULL;
        Tile last_tile = snake->head;
        while (last_tile->next != NULL) {
            parent = last_tile;
            last_tile = last_tile->next;
        }
        if (parent != NULL) {
            parent->next = NULL;
        }
        destroy_tile(last_tile);
        if (snake->length == 1) {
            snake->head = NULL;
        }
    }
    snake->got_bigger = 0;

    Tile new_head = create_tile(new_x, new_y, TILE_SIZE, snake->color);
    new_head->next = snake->head;
    snake->head = new_head;
}

void destroy_snake(Snake snake) { free(snake); }