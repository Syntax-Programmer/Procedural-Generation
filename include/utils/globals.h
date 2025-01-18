#pragma once

#include <math.h>

#define FRAME_MIN_TIME (1000.0 / 200) // Corresponds to delta time with 200 fps = 5 ms.

#define SCREEN_HEIGHT 800
#define SCREEN_WIDTH 1200
#define SCREEN_TITLE "A-RPG"

#define TILE_SIZE 10
#define CHUNK_SIZE 250
#define ROW_C (ceil(SCREEN_HEIGHT / CHUNK_SIZE) + 2)
#define COL_C (ceil(SCREEN_WIDTH / CHUNK_SIZE) + 2)

#define PERLIN_OCTAVES 5
#define PERLIN_TERRAIN_FREQ 0.012

#define SET_FLAG(var, flag)   ((var) |= (flag))
#define CLEAR_FLAG(var, flag) ((var) &= ~(flag))
#define TOGGLE_FLAG(var, flag) ((var) ^= (flag))
#define HAS_FLAG(var, flag)   (((var) & (flag)) ? 1 : 0)

typedef enum InputFlags{
    KEY_QUIT = 1, KEY_ESC = 2, KEY_UP = 4, KEY_DOWN = 8, KEY_RIGHT = 16,
    KEY_LEFT = 32, KEY_SHIFT = 64, KEY_CTRL = 128, KEY_SPACE = 256,
    MSE_LEFT = 512, MSE_RIGHT = 1024, MSE_SCRL = 2048
} InputFlags;