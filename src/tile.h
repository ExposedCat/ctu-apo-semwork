#ifndef TILE_H
#define TILE_H

#include <stdlib.h>

#include "screen.h"

#define Tile Tile_t*

typedef struct Tile_t {
    int x;
    int y;
    Color color;
    struct Tile_t* next;
} Tile_t;

Tile create_tile(int x, int y, Color color);

void render_tile(Screen screen, Tile tile);

int collides_with_tile(Tile tile, int x, int y);

void destroy_tile(Tile tile);

void move_tile(Tile tile, int x, int y);

void add_tile(Tile tile, Tile new_tile);

#endif