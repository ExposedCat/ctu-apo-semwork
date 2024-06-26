
#include "tile.h"

#include <stdio.h>

Tile create_tile(int x, int y, int size, Color color) {
    Tile tile = (Tile)malloc(sizeof(Tile_t));
    if (tile == NULL) {
        return NULL;
    }
    tile->x = x;
    tile->y = y;
    tile->color = color;
    tile->size = size;
    tile->next = NULL;
    return tile;
}

void render_tile(Screen screen, Tile tile) {
    int offset = (TILE_SIZE - tile->size) / 2;
    draw_scaled_pixel(screen, tile->x + offset, tile->y + offset, tile->size,
                      tile->color);
}

int collides_with_tile(Tile tile, int x, int y) {
    return x >= tile->x && x <= tile->x + TILE_SIZE && y >= tile->y &&
           y <= tile->y + TILE_SIZE;
}

void add_tile(Tile tile, Tile new_tile) {
    Tile target = tile;
    while (target->next != NULL) {
        target = target->next;
    }
    target->next = new_tile;
}

void destroy_tile(Tile tile) {
    Tile target = tile;
    while (target->next != NULL) {
        Tile tmp = target->next;
        free(target);
        target = tmp;
    }
    free(target);
}

void move_tile(Tile tile, int x, int y) {
    tile->x = x;
    tile->y = y;
}