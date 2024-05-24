#ifndef SCREEN_H
#define SCREEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "font_types.h"
#include "mzapo_regs.h"
#include "utils/colors.h"

#define TILE_SIZE 20
#define FOOD_SIZE 10

#define SCREEN_WIDTH 480
#define CENTER_X ((SCREEN_WIDTH - TILE_SIZE) / 2)
#define SCREEN_HEIGHT 320
#define PANEL_HEIGHT 20
#define PLAYGROUND_HEIGHT \
    (SCREEN_HEIGHT - PANEL_HEIGHT)  // NOTE: Ensure PLAYGROUND_HEIGHT is a
                                    // multiple of TILE_SIZE
#define CENTER_Y ((SCREEN_HEIGHT - TILE_SIZE) / 2)

#define Screen unsigned short *
#define Font font_descriptor_t *

void draw_pixel(Screen screen, int x, int y, Color color);

void draw_scaled_pixel(Screen screen, int x, int y, int scale,
                       unsigned short color);

void clear_screen(Screen screen);

void draw_string(Screen screen, int x, int y, const char *str, Color color);

void choose_random_tile(int *x, int *y);

void reset_screen(Screen screen);

#endif