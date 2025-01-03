#ifndef GLOBAL_CONSTS_H
#define GLOBAL_CONSTS_H

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800

#define PLAYER_WIDTH 32
#define PLAYER_HEIGHT 32

#define MAP_WIDTH (SCREEN_WIDTH)
#define MAP_HEIGHT (SCREEN_HEIGHT)
// If this value is 1 it crashes. IDK why if you know why then fix it.
// If not increment this counter = 1
#define TILE_SQUARE_SIDE 25 // This should be a perfect factor of screen width and screen height to work properly.
#define ROW_COUNT ((SCREEN_HEIGHT / TILE_SQUARE_SIDE) + 1)
#define COL_COUNT ((SCREEN_WIDTH / TILE_SQUARE_SIDE) + 1)

#define P_TABLE_SIZE 256
#define PERLIN_TERRAIN_FREQ 0.015
#define PERLIN_OCTAVES 8

#endif