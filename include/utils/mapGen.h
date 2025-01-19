#pragma once

#include <SDL2/SDL.h>
#include "utils/globals.h"
#include "utils/phy.h"
#include "utils/obj.h"

#define UPDATE_TOP 1
#define UPDATE_BOTTOM 2
#define UPDATE_RIGHT 4
#define UPDATE_LEFT 8


typedef struct Chunk {
    SDL_Texture *txtr;
    SDL_Rect dest;
} Chunk;


extern void freeChunkMap(Chunk **to_free);
extern Chunk **initTerrainMap(SDL_Renderer *renderer, int seed);