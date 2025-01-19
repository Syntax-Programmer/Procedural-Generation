#pragma once

#include <math.h>

#define FRAME_MIN_TIME (1000.0 / 100) // Corresponds to delta time with 100 fps = 10 ms.

#define SCREEN_HEIGHT 800
#define SCREEN_WIDTH 1200
#define SCREEN_TITLE "A-RPG"

/*
NOTE: Use TILE_SIZE such that TILE_SIZE % CHUNK_SIZE == 0.
If not, the way the create SDL_Texture in mapGen.C is setup,
it will cause a heap buffer overflow.
We assue that TILE_SIZE % CHUNK_SIZE == 0 as to ensure that all the pixel are filled in
with the appropriate color.
*/
#define TILE_SIZE 1
#define CHUNK_SIZE 5

#if (CHUNK_SIZE % TILE_SIZE != 0)
#error "TILE_SIZE must divide CHUNK_SIZE evenly due to the way create texture is made.!"
#endif

#define ROW_C (int)(ceil(SCREEN_HEIGHT / CHUNK_SIZE) + 2)
#define COL_C (int)(ceil(SCREEN_WIDTH / CHUNK_SIZE) + 2)

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