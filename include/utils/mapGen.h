#pragma once

#include <SDL2/SDL.h>
#include "utils/globals.h"
#include "utils/phy.h"

typedef struct Chunk {
    SDL_Texture *txtr;
    SDL_Rect dest;
} Chunk;

extern Chunk createChunk(SDL_Renderer *renderer, int cam_x, int cam_y,
                         int scrn_x, int scrn_y, int seed, float freq);
extern void freeChunkMap(Chunk **to_free);
extern Chunk **initTerrainMap(SDL_Renderer *renderer, int seed);