#include "../include/mapMgr.h"

/// @brief Applies random color perlin noise to the given texture.
/// @param to_edit The texture to edit with the noise.
/// @param seed The random seed the noise follows.
/// @param global_x The global x position of the top left corner of the texture.
/// @param global_y The global y position of the top left corner of the texture.
static void noiseToTerrainTexture(SDL_Texture *to_edit, time_t seed,
                                  int32_t global_x, int32_t global_y);

static void noiseToTerrainTexture(SDL_Texture *to_edit, time_t seed,
                                  int32_t global_x, int32_t global_y) {
    uint8_t *pixels = NULL;
    int32_t pitch = 0, grid_x = 0, grid_y = 0;
    uint32_t color = 0, tile_row_bffr[TILE_SIZE];

    SDL_LockTexture(to_edit, NULL, (void **)&pixels, &pitch);
    for (int y = 0; y < CHUNK_SIZE; y += TILE_SIZE) {
        for (int x = 0; x <  CHUNK_SIZE; x += TILE_SIZE) {
            grid_x = (global_x + x) / TILE_SIZE;
            grid_y = (global_y + y) / TILE_SIZE;
            color = FBM(PERLIN_TERRAIN_FREQ, grid_x, grid_y, seed);
            for (int i = 0; i < TILE_SIZE; i++) {
                tile_row_bffr[i] = color;
            }
            for (int i = 0; i < TILE_SIZE; i++) { // Filling in the tile row-wise
                memcpy(pixels + (y + i) * pitch + x * 4, tile_row_bffr, TILE_SIZE * 4);
            }
        }
    }
    SDL_UnlockTexture(to_edit);
}

void initTerrainMap(SDL_Renderer *renderer, SDL_Texture ***pTerrain_map,
                    PosHandle *pMap_pos_handle, time_t seed) {
    int32_t scrn_x = -CHUNK_SIZE, scrn_y = -CHUNK_SIZE;

    *pTerrain_map = malloc(CHUNK_C * sizeof(SDL_Texture *));
    if (!(*pTerrain_map)) {
        SDL_Log("Couldn't allocate mem for SDL_Texture array. Malloc failure.");
        return;
    }
    pMap_pos_handle->cam_x = pMap_pos_handle->cam_y = 0;
    pMap_pos_handle->origin_chunk_x = scrn_x;
    pMap_pos_handle->origin_chunk_y = scrn_y;
    for (int i = 0; i < CHUNK_C; i++) {
        if (i && !(i % COL_C)) { // Not 0 and next row.
            scrn_y += CHUNK_SIZE;
            scrn_x = -CHUNK_SIZE;
        }
        (*pTerrain_map)[i] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                             SDL_TEXTUREACCESS_STREAMING, CHUNK_SIZE, CHUNK_SIZE);
        if (!(*pTerrain_map)[i]) { continue; } // The texture will be blank till it is recreated.
        noiseToTerrainTexture((*pTerrain_map)[i], seed, scrn_x, scrn_y);
        scrn_x += CHUNK_SIZE;
    }
}

void freeTerrainMap(SDL_Texture **terrain_map) {
    if (!terrain_map) { return; }
    for (int i = 0; i < CHUNK_C; i++) {
        if (terrain_map[i]) {
            SDL_DestroyTexture(terrain_map[i]);
            terrain_map[i] = NULL;
        }
    }
    free(terrain_map);
}
