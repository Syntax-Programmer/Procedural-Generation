#ifndef MAP_HANDLER_H
#define MAP_HANDLER_H

#include "utils/physics.h"
#include "utils/obj.h"
#include "SDL2/SDL.h"

typedef struct ChunkData {
    SDL_Texture *texture;
    SDL_FRect dest;
} ChunkData;

extern ChunkData createTextureChunk(SDL_Renderer *renderer, float start_x, float start_y, int seed, float freq);
extern void freeTerrainTextureMap(ChunkData **to_free);
extern ChunkData **initTerrainTextureMap(SDL_Renderer *renderer, int seed);

#endif // MAP_HANDLER_H