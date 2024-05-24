#include "image.h"

int read_asset(char* asset_name, Image* image) {
    *image = NULL;
    FILE* image_file = fopen(asset_name, "rb");
    if (!image_file) {
        fprintf(stderr, "[CRITICAL]: Cannot open asset \"%s\"\n", asset_name);
        return 1;
    }

    int width = 0;
    int height = 0;
    int _ = 0;

    // Skip P6 line
    fseek(image_file, 3, SEEK_SET);

    int read_entries = fscanf(image_file, "%d %d %d", &width, &height, &_);
    if (read_entries < 3) {
        printf("[CRITICAL] Ensure asset \"%s\" is a .PPM image\n", asset_name);
        return 1;
    }
    if (width != TILE_SIZE || height != TILE_SIZE) {
        printf("[CRITICAL] Asset \"%s\" is of a wrong size (not %dx%d)\n",
               asset_name, TILE_SIZE, TILE_SIZE);
        return 1;
    }

    // Skip newline
    fgetc(image_file);

    // PPM images are arrays of [R, G, B] sequences
    *image = (unsigned char*)malloc(TILE_SIZE * TILE_SIZE * 3 *
                                    sizeof(unsigned char));

    read_entries =
        fread(*image, sizeof(char), TILE_SIZE * TILE_SIZE * 3, image_file);
    if (read_entries != TILE_SIZE * TILE_SIZE * 3) {
        return 1;
    }

    fclose(image_file);

    return 0;
}

int render_asset(char* asset_name, Screen screen, int x, int y) {
    Image image = NULL;
    int read_result = read_asset(asset_name, &image);
    if (read_result) {
        return read_result;
    }

    for (int shift_y = 0; shift_y < TILE_SIZE; ++shift_y) {
        for (int shift_x = 0; shift_x < TILE_SIZE; ++shift_x) {
            int r = image[(shift_y * TILE_SIZE + shift_x) * 3];
            int g = image[(shift_y * TILE_SIZE + shift_x) * 3 + 1];
            int b = image[(shift_y * TILE_SIZE + shift_x) * 3 + 2];

            int coordinate = (x + shift_x) + SCREEN_WIDTH * (y + shift_y);
            if (r != 0 || g != 0 || b != 0) {
                screen[coordinate] = ((r & 0b11111000) << 8) |
                                     ((g & 0b11111100) << 3) | (b >> 3);
            } else {
                screen[coordinate] = 0x0;
            }
        }
    }

    free(image);
    return 0;
}
