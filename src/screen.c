#include "screen.h"

void draw_pixel(Screen screen, int x, int y, Color color) {
    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
        screen[x + SCREEN_WIDTH * y] = color;
    }
}

void draw_scaled_pixel(Screen screen, int x, int y, int scale,
                       unsigned short color) {
    for (int i = 0; i < scale; ++i) {
        for (int j = 0; j < scale; ++j) {
            draw_pixel(screen, x + i, y + j, color);
        }
    }
}

void clear_screen(Screen screen) {
    for (int pixel = 0; pixel < 320 * 480; ++pixel) {
        screen[pixel] = 0xFFFF;
    }
}

int character_width(int character, Font font) {
    int width = 0;
    if ((character >= font->firstchar) &&
        (character - font->firstchar < font->size)) {
        character -= font->firstchar;
        if (!font->width) {
            width = font->maxwidth;
        } else {
            width = font->width[character];
        }
    }
    return width;
}

void draw_character(Screen screen, int x, int y, char character, Font font,
                    Color color) {
    for (unsigned w = 0; w < font->maxwidth; ++w) {
        for (unsigned h = 0; h < font->height; ++h) {
            if (font->bits[(character - font->firstchar) * font->height + h] &
                (1 << (16 - w))) {
                screen[x + w + SCREEN_WIDTH * (y + h)] = color;
            }
        }
    }
}

void draw_string(Screen screen, int x, int y, const char *text, Color color) {
    for (unsigned i = 0; i < strlen(text); ++i) {
        draw_character(screen, x + i * (&font_rom8x16)->maxwidth, y, text[i],
                       &font_rom8x16, color);
    }
}

void choose_random_tile(int *x, int *y) {
    srand(time(NULL));

    int num_tiles_x = (SCREEN_WIDTH / TILE_SIZE) - 1;
    int num_tiles_y = (SCREEN_HEIGHT / TILE_SIZE) - 1;

    int tile_index_x = rand() % num_tiles_x;
    int tile_index_y = rand() % num_tiles_y;

    *x = tile_index_x * TILE_SIZE;
    *y = tile_index_y * TILE_SIZE;
}

void reset_screen(Screen screen) {
    for (int pixel = 0; pixel < SCREEN_HEIGHT * SCREEN_WIDTH; pixel++) {
        screen[pixel] = 0u;
    }
}