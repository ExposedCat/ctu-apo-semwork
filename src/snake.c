#include "snake.h"

#include "food.h"

Snake create_snake(int length, Color color, int x, int y, Direction direction) {
    Snake snake = (Snake)malloc(sizeof(Snake_t));
    if (snake == NULL) {
        return NULL;
    }

    Tile head = (Tile)malloc(sizeof(Tile_t));
    if (head == NULL) {
        free(snake);
        return NULL;
    }

    snake->length = length;
    snake->color = color;
    snake->head = head;
    snake->direction = direction;
    return snake;
}

void render_snake(Screen screen, Snake snake) {
    draw_scaled_pixel(screen, snake->head->x, snake->head->y, TILE_SIZE,
                      snake->color);
}

int collides_with_snake(Snake snake, int x, int y) {
    Tile tile = snake->head;
    do {
        if (x >= tile->x && x <= tile->x + TILE_SIZE && y >= tile->y &&
            y <= tile->y + TILE_SIZE) {
            return 1;
        }
        if (tile->next != NULL) {
            tile = tile->next;
        } else {
            break;
        }
    } while (1);
    return 0;
}

void ensure_snake_collisions(Snake snake, Snake another_snake, Food food) {
    if (collides_with_food(food, snake->head->x, snake->head->y)) {
        snake->length += 1;
        move_food(food);
        printf("[SNAKE] Ate food");
    }

    if (collides_with_snake(another_snake, snake->head->x, snake->head->y)) {
        printf("[SNAKE] Dead");
    }
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
    snake->head->x += change_x * TILE_SIZE;
    if (snake->head->x < 0) {
        snake->head->x += SCREEN_WIDTH;
    }
    if (snake->head->x > SCREEN_WIDTH) {
        snake->head->x -= SCREEN_WIDTH;
    }

    snake->head->y += change_y * TILE_SIZE;
    if (snake->head->y < 0) {
        snake->head->y += SCREEN_HEIGHT;
    }
    if (snake->head->y > SCREEN_HEIGHT) {
        snake->head->y -= SCREEN_HEIGHT;
    }
}

void destroy_snake(Snake snake) { free(snake); }