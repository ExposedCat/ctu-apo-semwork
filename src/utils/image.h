#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../screen.h"

#define Image unsigned char*
#define ImagePixel ImagePixel_t*

typedef struct ImagePixel_t {
    int x;
    int y;
    uint16_t color;
} ImagePixel_t;

int render_asset(char* asset_name, Screen screen, int x, int y);

#endif