/*******************************************************************
 classic game of snakes
 by Ievgeniia Rachkovska and Artem Prokop


 *******************************************************************/

#define _POSIX_C_SOURCE 200112L

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>  //termios, TCSANOW, ECHO, ICANON
#include <time.h>
#include <unistd.h>

#include "font_types.h"
#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "src/snake.h"

#define M_PI 3.1415

#define SPILED_REG_LED_LINE_o 0x00  // Adjust this offset as needed

typedef struct rectangle {
    size_t x;
    size_t y;
    struct rectangle *next;
} rectangle_t;

typedef enum {
    RED,
    BLUE
} Colours_t;  // colors don`t work like I wanted. Maybe needed to change

typedef enum { UP, DOWN, LEFT, RIGHT } Directions_t;

typedef struct {
    rectangle_t *head;
    size_t snake_size;  // so it is score
    Colours_t colour;
    Directions_t direction;
} Snake_t;

typedef struct {
    rectangle_t food;
    size_t prize_size;  // I think it will be always 1
    Colours_t colour;
} Food_t;

unsigned short *fb;
font_descriptor_t *fdes;
int scale = 1;

void draw_scaled_pixel(int x, int y, unsigned short color) {
    int i, j;
    for (i = 0; i < scale; i++) {
        for (j = 0; j < scale; j++) {
            draw_pixel(x + i, y + j, color);
        }
    }
}

void move_rectangle(int x, int y, unsigned short color) {}

int char_width(int ch) {
    int width;
    if (!fdes->width) {
        width = fdes->maxwidth;
    } else {
        width = fdes->width[ch - fdes->firstchar];
    }
    return width;
}

void draw_char(int x, int y, char ch, unsigned short color) {
    int w = char_width(ch);
    const font_bits_t *ptr;
    if ((ch >= fdes->firstchar) && (ch - fdes->firstchar < fdes->size)) {
        if (fdes->offset) {
            ptr = &fdes->bits[fdes->offset[ch - fdes->firstchar]];
        } else {
            int bw = (fdes->maxwidth + 15) / 16;
            ptr = &fdes->bits[(ch - fdes->firstchar) * bw * fdes->height];
        }
        int i, j;
        for (i = 0; i < fdes->height; i++) {
            font_bits_t val = *ptr;
            for (j = 0; j < w; j++) {
                if ((val & 0x8000) != 0) {
                    draw_scaled_pixel(x + scale * j, y + scale * i, color);
                }
                val <<= 1;
            }
            ptr++;
        }
    }
}

void draw_string(int x, int y, const char *str, unsigned short color) {
    while (*str) {
        draw_char(x, y, *str++, color);
        x += char_width(*str);  // Move x to the next character position
    }
}
void draw_score(int score, int x, int y, unsigned short color) {
    char score_str[15];
    snprintf(score_str, sizeof(score_str), "S c o r e: %d", score);
    draw_string(x, y, score_str, color);
}

void draw_menu(unsigned short *fb, unsigned char *parlcd_mem_base) {
    // Clear the screen
    for (int ptr = 0; ptr < 320 * 480; ptr++) {
        fb[ptr] = 0xFFFF;  // White
                           // fb[ptr] = 0x0000; // Black
    }

    // Draw the menu options
    draw_string(100, 100, "Snake Game", 0x0000);                 // Black
    draw_string(100, 140, "Press any button to start", 0x0000);  // Black

    // // Display the menu
    parlcd_write_cmd(parlcd_mem_base, 0x2c);
    for (int ptr = 0; ptr < 480 * 320; ptr++) {
        parlcd_write_data(parlcd_mem_base, fb[ptr]);
    }
}

void wait_for_start(unsigned char *mem_base) {
    // Wait for user to press a button
    while (1) {
        int r = *(volatile uint32_t *)(mem_base + SPILED_REG_KNOBS_8BIT_o);
        if ((r & 0x100) != 0) {
            break;
        }
    }
}

void led_line(unsigned char *mem_base, uint32_t val_line) {
    for (int i = 0; i < 30; i++) {
        *(volatile uint32_t *)(mem_base + SPILED_REG_LED_LINE_o) = val_line;
        val_line <<= 1;
        printf("LED val 0x%x\n", val_line);
        sleep(0.6);
    }
}

void initialize_snake(Snake_t *snake, size_t x, size_t y,
                      Directions_t direction) {
    snake->head = malloc(sizeof(rectangle_t));
    if (snake->head == NULL) {
        exit(EXIT_FAILURE);
    }
    snake->head->x = x;
    snake->head->y = y;
    snake->head->next = NULL;
    snake->snake_size = 1;
    snake->direction = direction;
}

void initialize_food(Food_t *food, size_t x, size_t y, Colours_t colour) {
    food->food.x = x;
    food->food.y = y;
    food->prize_size = 1;
    food->colour = colour;
}

void draw_snake(Snake_t *snake, unsigned short color) {
    rectangle_t *segment = snake->head;
    while (segment != NULL) {
        for (int j = 0; j < 30; j++) {
            for (int i = 0; i < 30; i++) {
                draw_pixel(segment->x + i, segment->y + j, color);
            }
        }
        segment = segment->next;
    }
}

void draw_food(Food_t *food, unsigned short color) {
    int x = food->food.x;
    int y = food->food.y;
    for (int j = 0; j < 30; j++) {
        for (int i = 0; i < 30; i++) {
            draw_pixel(x + i, y + j, color);
        }
    }
}

// in the standard variant two snakes, the direction of one head controlled
// by red selector and the characters 'a' and 's' in the serial port input,
// the second head with the blue selector and the characters 'k' and 'l'

// manupulation with knobs is +- working (red and blue sectors)
// it depends on the start position of knobs (maybe it shouldn`t)
//  TO DO:
// control by characters
void change_direction(Snake_t *snake, int delta) {
    if (delta < -128 || (delta > 0 && delta < 128)) {
        // zz has increased or significantly decreased (passed zero)
        switch (snake->direction) {
            case RIGHT:
                snake->direction = DOWN;
                break;
            case LEFT:
                snake->direction = UP;
                break;
            case UP:
                snake->direction = RIGHT;
                break;
            case DOWN:
                snake->direction = LEFT;
                break;
            default:
                break;
        }
    } else if (delta > 128 || (delta < 0 && delta > -128)) {
        // zz has decreased or significantly increased (passed 255)
        switch (snake->direction) {
            case RIGHT:
                snake->direction = UP;
                break;
            case LEFT:
                snake->direction = DOWN;
                break;
            case UP:
                snake->direction = LEFT;
                break;
            case DOWN:
                snake->direction = RIGHT;
                break;
            default:
                break;
        }
    }
}
void update_snake_position(Snake_t *snake) {
    // Move each segment of the snake based on its current direction
    rectangle_t *segment = snake->head;

    while (segment != NULL) {
        // Move the segment based on the current direction
        switch (snake->direction) {
            case UP:
                segment->y--;
                break;
            case DOWN:
                segment->y++;
                break;
            case LEFT:
                segment->x--;
                break;
            case RIGHT:
                segment->x++;
                break;
            default:
                break;
        }

        // Move to the next segment
        segment = segment->next;
    }
}

// Function to update the positions of all snake segments relative to the head
void update_snake_segments(Snake_t *snake) {
    rectangle_t *segment = snake->head->next;  // Start from the second segment
    size_t x = snake->head->x;
    size_t y = snake->head->y;

    // Update each segment's position relative to the head
    while (segment != NULL) {
        switch (snake->direction) {
            case UP:
                segment->x = x;
                segment->y = y + (30 * snake->snake_size);
                break;
            case DOWN:
                segment->x = x;
                segment->y = y - (30 * snake->snake_size);
                break;
            case LEFT:
                segment->x = x + (30 * snake->snake_size);
                segment->y = y;
                break;
            case RIGHT:
                segment->x = x - (30 * snake->snake_size);
                segment->y = y;
                break;
            default:
                break;
        }
        segment = segment->next;
        snake->snake_size--;
    }
}

// Updated grow_snake function to create new segments and update positions
// it is not working good the snake is moving and it can add segment in a bad
// place
void grow_snake(Snake_t *snake) {
    // Calculate new segment position based on the last segment's position and
    // the current direction
    size_t new_x = 0, new_y = 0;
    rectangle_t *last_segment = snake->head;
    // Increment the snake's size
    snake->snake_size++;
    printf("I grow\n");

    // Traverse the snake to find the last segment
    while (last_segment->next != NULL) {
        last_segment = last_segment->next;
    }

    // Calculate new segment position based on the last segment's position and
    // the current direction
    switch (snake->direction) {
        case UP:
            new_x = last_segment->x;
            new_y = last_segment->y - 30;
            break;
        case DOWN:
            new_x = last_segment->x;
            new_y = last_segment->y + 30;
            break;
        case LEFT:
            new_x = last_segment->x - 30;
            new_y = last_segment->y;
            break;
        case RIGHT:
            new_x = last_segment->x + 30;
            new_y = last_segment->y;
            break;
        default:
            break;
    }

    // Create a new segment
    rectangle_t *new_segment = malloc(sizeof(rectangle_t));
    if (new_segment == NULL) {
        exit(EXIT_FAILURE);
    }
    new_segment->x = new_x;
    new_segment->y = new_y;
    new_segment->next = NULL;

    // Add the new segment after the last segment
    last_segment->next = new_segment;
}

// maybe needs some changes
//  int check_collision(Snake_t *snake, Food_t *food) {
//      return (snake->head->x == food->food.x && snake->head->y ==
//      food->food.y);
//  }
int check_collision(Snake_t *snake, Food_t *food) {
    int snake_x1 = snake->head->x;
    int snake_y1 = snake->head->y;
    int snake_x2 = snake->head->x + 30;  // Assuming each snake segment is 30x30
    int snake_y2 = snake->head->y + 30;

    int food_x1 = food->food.x;
    int food_y1 = food->food.y;
    int food_x2 = food->food.x + 30;  // Assuming the food is 30x30
    int food_y2 = food->food.y + 30;

    // Check for overlap
    return (snake_x1 < food_x2 && snake_x2 > food_x1 && snake_y1 < food_y2 &&
            snake_y2 > food_y1);
}

void relocate_food(Food_t *food) {
    food->food.x = (rand() % 16) * 30;  // Assuming the grid is 16x10
    food->food.y = (rand() % 10) * 30;
}

int main(int argc, char *argv[]) {
    unsigned char *parlcd_mem_base, *mem_base;
    int i, j;
    int ptr;
    unsigned int c;
    fb = (unsigned short *)malloc(320 * 480 * 2);
    uint32_t val_line = 3;

    printf("[MAIN] Program started\n");

    // some things so programe is working
    // I don`t know what is that
    parlcd_mem_base =
        map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
    if (parlcd_mem_base == NULL)
        exit(1);

    mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
    if (mem_base == NULL)
        exit(1);

    parlcd_hx8357_init(parlcd_mem_base);

    parlcd_write_cmd(parlcd_mem_base, 0x2c);
    ptr = 0;
    for (i = 0; i < 320; i++) {
        for (j = 0; j < 480; j++) {
            c = 0;
            fb[ptr] = c;
            parlcd_write_data(parlcd_mem_base, fb[ptr++]);
        }
    }
    fdes = &font_winFreeSystem14x16;

    // Init and lifecycle of the game
    draw_menu(fb, parlcd_mem_base);
    wait_for_start(mem_base);

    Snake_t snake1, snake2;
    initialize_snake(&snake1, 100, 100, RIGHT);
    initialize_snake(&snake2, 480 - 30, 320 - 30, LEFT);

    Food_t food;
    initialize_food(&food, 200, 150, RED);

    struct timespec loop_delay;
    loop_delay.tv_sec = 0;
    loop_delay.tv_nsec = 250 * 1000 * 1000;
    int xx = 0, yy = 0, zz = 0;
    int oldxx = 0, oldzz = 0;
    while (1) {
        int r = *(volatile uint32_t *)(mem_base + SPILED_REG_KNOBS_8BIT_o);
        if ((r & 0x7000000) != 0) {
            break;
        }
        xx = ((r & 0xff) * 480) / 256;  // blue
        // yy = (((r>>8)&0xff)*320)/256;//green
        zz = (((r >> 16) & 0xff) * 160) / 256;  // red

        int delta1 = zz - oldzz;
        int delta2 = xx - oldxx;
        change_direction(&snake1, delta1);
        change_direction(&snake2, delta2);

        if (snake1.snake_size > 1) {
            update_snake_segments(&snake1);
        }
        if (snake2.snake_size > 1) {
            update_snake_segments(&snake2);
        }

        oldxx = xx;
        oldzz = zz;

        for (ptr = 0; ptr < 320 * 480; ptr++) {
            fb[ptr] = 0u;
        }

        update_snake_position(&snake1);
        update_snake_position(&snake2);

        draw_snake(&snake1, 0xFF00);
        draw_snake(&snake2, 0x001F);
        draw_food(&food, 0x07E0);  // Green food

        draw_score(snake1.snake_size, 10, 10, 0xFFFF);  // White color
        printf("snake1 score %d\n", snake1.snake_size);

        if (check_collision(&snake1, &food)) {
            printf("eat\n");
            grow_snake(&snake1);
            relocate_food(&food);
        }

        parlcd_write_cmd(parlcd_mem_base, 0x2c);
        for (ptr = 0; ptr < 480 * 320; ptr++) {
            parlcd_write_data(parlcd_mem_base, fb[ptr]);
        }

        clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    }

    parlcd_write_cmd(parlcd_mem_base, 0x2c);
    for (ptr = 0; ptr < 480 * 320; ptr++) {
        parlcd_write_data(parlcd_mem_base, 0);
    }

    // led_line(mem_base, val_line);

    printf("[MAIN] Program finished\n");

    return 0;
}