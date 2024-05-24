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
    int width;
    if (!font->width) {
        width = font->maxwidth;
    } else {
        width = font->width[character - font->firstchar];
    }
    return width;
}

void draw_character(Screen screen, int x, int y, char character, Font font,
                    Color color) {
    int scale = 1;
    int width = character_width(character, font);
    const font_bits_t *bit;
    if ((character >= font->firstchar) &&
        (character - font->firstchar < font->size)) {
        if (font->offset) {
            bit = &font->bits[font->offset[character - font->firstchar]];
        } else {
            int bw = (font->maxwidth + 15) / 16;
            bit =
                &font->bits[(character - font->firstchar) * bw * font->height];
        }
        for (int i = 0; i < font->height; ++i) {
            font_bits_t val = *bit;
            for (int j = 0; j < width; ++j) {
                if ((val & 0x8000) != 0) {
                    draw_scaled_pixel(screen, x + scale * j, y + scale * i,
                                      scale, color);
                }
                val <<= 1;
            }
            bit++;
        }
    }
}

void draw_string(Screen screen, Font font, int x, int y, const char *str,
                 Color color) {
    while (*str) {
        draw_character(screen, x, y, *str++, font, color);
        x += character_width(*str, font);
    }
}

void draw_menu(Screen screen, Font font, unsigned char *parlcd_mem_base) {
    clear_screen(screen);

    draw_string(screen, font, 100, 100, "Snake Game", BLACK);
    draw_string(screen, font, 100, 140, "Press any button to start", BLACK);

    parlcd_write_cmd(parlcd_mem_base, 0x2c);
    for (int pixel = 0; pixel < 480 * 320; ++pixel) {
        parlcd_write_data(parlcd_mem_base, screen[pixel]);
    }
}

void wait_for_start(unsigned char *mem_base) {
    while (1) {
        int r = *(volatile uint32_t *)(mem_base + SPILED_REG_KNOBS_8BIT_o);
        if ((r & 0x100) != 0) {
            break;
        }
    }
}

void choose_random_tile(int *x, int *y) {
    srand(time(NULL));

    int num_tiles_x = SCREEN_WIDTH / TILE_SIZE;
    int num_tiles_y = SCREEN_HEIGHT / TILE_SIZE;

    int tile_index_x = rand() % num_tiles_x;
    int tile_index_y = rand() % num_tiles_y;

    *x = tile_index_x * TILE_SIZE;
    *y = tile_index_y * TILE_SIZE;
}