#include "utils/map_handler.h"

ChunkData createTextureChunk(SDL_Renderer *renderer, float start_x, float start_y, int seed, float freq) {
    SDL_FRect temp;
    uint8_t r, g, b;
    const uint8_t A = 255;
    ChunkData data = {.texture = NULL};
    data.dest = (SDL_FRect){.x = start_x, .y = start_y, .w = CHUNKSIZE, .h = CHUNKSIZE};
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
                                             CHUNKSIZE, CHUNKSIZE);

    if (!texture) {
        fprintf(stderr, "ERROR: Couldn't create chunk texture.\n");
        return data;
    }
    if (SDL_SetRenderTarget(renderer, texture)) {
        fprintf(stderr, "ERROR: Couldn't create chunk texture.\n");
        SDL_DestroyTexture(texture);
        data.texture = NULL;
        return data;
    }
    SDL_RenderClear(renderer);
    for (int y = 0; y < CHUNKSIZE; y++) {
        for (int x = 0; x < CHUNKSIZE; x++) {
            FBM(freq, start_x + x, start_y + y, seed, &r, &g, &b);
            setRectPos(&temp, x, y);
            setRectSize(&temp, TILE_SQUARE_SIDE, TILE_SQUARE_SIDE);
            SDL_SetRenderDrawColor(renderer, r, g, b, A);
            SDL_RenderFillRectF(renderer, &temp);
        }
    }
    SDL_SetRenderTarget(renderer, NULL);
    data.texture = texture;
    return data;
}

void freeTerrainTextureMap(ChunkData **to_free) {
    for (int y = 0; y < ROW_CHUNK_COUNT && to_free[y]; y++) {
        for (int x = 0; x < COL_CHUNK_COUNT && to_free[y][x].texture; x++) {
            SDL_DestroyTexture(to_free[y][x].texture);
            to_free[y][x].texture = NULL;
        }
        free(to_free[y]);
        to_free[y] = NULL;
    }
    free(to_free);
}

ChunkData **initTerrainTextureMap(SDL_Renderer *renderer, int seed) {
    ChunkData **terrain_map = malloc(ROW_CHUNK_COUNT * sizeof(ChunkData *));
    if (!terrain_map) {
        fprintf(stderr, "ERROR: Failed to allocated memory for the terrain map init.\n");
        return NULL;
    }
    // Starting from -1 to maintain padding from all sides.
    for (int y = -1; y < ROW_CHUNK_COUNT - 1; y++) {
        terrain_map[y + 1] = malloc(COL_CHUNK_COUNT * sizeof(ChunkData));
        if (!terrain_map[y + 1]) {
            fprintf(stderr, "ERROR: Failed to allocated memory for col in the terrain map init.\n");
            freeTerrainTextureMap(terrain_map);
            return NULL;
        }
        for (int x = -1; x < COL_CHUNK_COUNT - 1; x++) {
            terrain_map[y + 1][x + 1] = createTextureChunk(renderer, x * CHUNKSIZE, y * CHUNKSIZE, seed, PERLIN_TERRAIN_FREQ);
            if (!terrain_map[y + 1][x + 1].texture) {
                freeTerrainTextureMap(terrain_map);
                return NULL;
            }
        }
    }
    return terrain_map;
}