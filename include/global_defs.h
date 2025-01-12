#ifndef GLOBAL_DEFS_H
#define GLOBAL_DEFS_H

#include <math.h>

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800

#define TILE_SQUARE_SIDE 1
#define CHUNKSIZE (TILE_SQUARE_SIDE * 10) // Don't remove the ()
// The + (CHUNKSIZE * 2) is for extra padding for smooth updates.
#define ROW_CHUNK_COUNT ((SCREEN_HEIGHT / CHUNKSIZE) + 2)
#define COL_CHUNK_COUNT ((SCREEN_WIDTH / CHUNKSIZE) + 2)

#define MOVING_NOWHERE 0
#define MOVING_RIGHT 1
#define MOVING_LEFT 2
#define MOVING_UP 4
#define MOVING_DOWN 8

#define PERLIN_TERRAIN_FREQ 0.007
#define PERLIN_OCTAVES 8

#endif // GLOBAL_DEFS_H