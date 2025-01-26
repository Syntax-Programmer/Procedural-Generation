#pragma once

#include <math.h>

#define FRAME_MIN_TIME (1000.0 / 100) // Corresponds to delta time with 100 fps = 10 ms.

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800
#define GAME_TITLE "A-RPG"

#define TILE_SIZE 1
#define CHUNK_SIZE 250

// The +2 for padding from either side.
#define ROW_C (int)(ceil(SCREEN_HEIGHT / CHUNK_SIZE) + 2)
#define COL_C (int)(ceil(SCREEN_WIDTH / CHUNK_SIZE) + 2)

#define PERLIN_TERRAIN_FREQ 0.01
#define OCTAVES 8

#define SET_FLAG(var, flag)   ((var) |= (flag))
#define CLEAR_FLAG(var, flag) ((var) &= ~(flag))
#define TOGGLE_FLAG(var, flag) ((var) ^= (flag))
#define HAS_FLAG(var, flag)   (((var) & (flag)) ? 1 : 0)

typedef enum InputFlags {
    UP = 1, DOWN = 2, LEFT = 4, RIGHT = 8, MSB_RIGHT = 16, MSB_LEFT = 32,
    ESC = 64, SPACE = 128, CTRL = 256, SHIFT = 512, QUIT = 1024
} InputFlags;