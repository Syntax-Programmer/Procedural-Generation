#ifndef GLOBAL_CONSTS_H
#define GLOBAL_CONSTS_H

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800
// If this value is 1 it crashes. IDK why if you know why then fix it.
// If not increment this counter = 1
// Fixed it. Me, 2 day later
#define TILE_SQUARE_SIDE 15
// The +2 accounts for some extra objects so if the player moves very fast the screen doesn't flicker.
#define ROW_COUNT ((SCREEN_HEIGHT / TILE_SQUARE_SIDE) + 2)
#define COL_COUNT ((SCREEN_WIDTH / TILE_SQUARE_SIDE) + 2)

#define P_TABLE_SIZE 256
#define PERLIN_TERRAIN_FREQ 0.007
#define PERLIN_OCTAVES 8

#endif