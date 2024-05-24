#ifndef SCREEN_H
#define SCREEN_H

#include <stdlib.h>
#include <time.h>

#include "font_types.h"
#include "mzapo_regs.h"
#include "utils.h"

#define TILE_SIZE 20
#define FOOD_SIZE 10

#define SCREEN_WIDTH 480
#define CENTER_X (SCREEN_WIDTH - TILE_SIZE) / 2
#define SCREEN_HEIGHT 320
#define CENTER_Y (SCREEN_HEIGHT - TILE_SIZE) / 2

#define Screen unsigned short *
#define Font font_descriptor_t *

void draw_pixel(Screen screen, int x, int y, Color color);
void draw_scaled_pixel(Screen screen, int x, int y, int scale,
                       unsigned short color);
void clear_screen(Screen screen);
void draw_character(Screen screen, int x, int y, char character, Font font,
                    Color color);
void draw_string(Screen screen, Font font, int x, int y, const char *str,
                 Color color);
void draw_menu(Screen screen, Font font, unsigned char *parlcd_mem_base);
void wait_for_start(unsigned char *mem_base);
void choose_random_tile(int *x, int *y);

#endif