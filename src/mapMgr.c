#include "../include/mapMgr.h"

/*
 * @brief Applies a perlin noise texture to the give SDL_Texture.
 *
 * Using the given seed and the given global pos of the texture, this
 * edits the pixel data of the texture directly and applies the random gradient
 * color to it.
 *
 * @param to_edit The SDL_Texture to apply the noise to.
 * @param seed The seed for the current session of this code running used to
 *             apply the random noise.
 * @param pGlobal_pos The pointer to the **IntVec2** global pos of the texture
 *                    in the game world, defined in **physics.h**.
 */
static void noiseToTerrainTexture(SDL_Texture *to_edit, time_t seed, IntVec2 *pGlobal_pos);

/*
 * @brief Check if the current state of the terrain_map require and update.
 *
 * Using the camera pos in the world, we find out if the player has moved 1 chunk
 * over in any direction, and if yes the terrain in that direction need updating.
 *
 * @note This function currently does not account for the player's **delta_dist**
 *       calculated in the **handleState()** in **A-RPG.c** not being 1 or 0 or -1.
 *       Currently not having any overflow/underflow margins for irregular delta_dist
 *       that may not be directly divisible to **CHUNK_SIZE** defined in **global.h**
 *
 * @param *pWorldPosHandler Used to manage map and cam positions of the game.
 * @param input_flags Bitmask of the inputs done by the user used to
 *                    determine what the player wants to do.
 *
 * @return Bitmask of the directions to update in the terrain_map from.
 */
static uint8_t getTerrainChunkUpdateFlag(PosHandle *pWorld_pos_handle, uint16_t input_flags);

static void noiseToTerrainTexture(SDL_Texture *to_edit, time_t seed, IntVec2 *pGlobal_pos) {
    uint8_t *pixels = NULL;
    int32_t pitch = 0;
    uint32_t color = 0, tile_row_bffr[TILE_SIZE];
    IntVec2 grid_pos;

    SDL_LockTexture(to_edit, NULL, (void **)&pixels, &pitch);
    for (int y = 0; y < CHUNK_SIZE; y += TILE_SIZE) {
        for (int x = 0; x <  CHUNK_SIZE; x += TILE_SIZE) {
            grid_pos.x = (pGlobal_pos->x + x) / TILE_SIZE;
            grid_pos.y = (pGlobal_pos->y + y) / TILE_SIZE;
            color = FBM(PERLIN_TERRAIN_FREQ, &grid_pos, seed);
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
    IntVec2 scrn_pos = {.x = -CHUNK_SIZE, .y = -CHUNK_SIZE};

    *pTerrain_map = malloc(CHUNK_C * sizeof(SDL_Texture *));
    if (!(*pTerrain_map)) {
        SDL_Log("Couldn't allocate mem for SDL_Texture array. Malloc failure.");
        return;
    }
    pMap_pos_handle->cam_pos.x = pMap_pos_handle->cam_pos.y = 0;
    pMap_pos_handle->origin_chunk_pos.x = scrn_pos.x;
    pMap_pos_handle->origin_chunk_pos.y = scrn_pos.y;
    for (int i = 0; i < CHUNK_C; i++) {
        if (i && !(i % COL_C)) { // Not 0 and next row.
            scrn_pos.y += CHUNK_SIZE;
            scrn_pos.x = -CHUNK_SIZE;
        }
        (*pTerrain_map)[i] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                             SDL_TEXTUREACCESS_STREAMING, CHUNK_SIZE, CHUNK_SIZE);
        if (!(*pTerrain_map)[i]) { continue; } // The texture will be blank till it is recreated.
        noiseToTerrainTexture((*pTerrain_map)[i], seed, &scrn_pos);
        scrn_pos.x += CHUNK_SIZE;
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

uint8_t getTerrainChunkUpdateFlag(PosHandle *pWorld_pos_handle, uint16_t input_flags) {
    //* This will currently only work with the velocities for which delta_dist is always 1/-1/0,
    //* This is because we don't have any overflow/underflow margins for irregular vel.

    uint8_t update_flags = 0;
    int32_t x_mod = pWorld_pos_handle->cam_pos.x % CHUNK_SIZE,
            y_mod = pWorld_pos_handle->cam_pos.y % CHUNK_SIZE;

    if (!x_mod) {
        if (HAS_FLAG(input_flags, RIGHT)) { SET_FLAG(update_flags, LEFT); }
        else if (HAS_FLAG(input_flags, LEFT)) { SET_FLAG(update_flags, RIGHT); }
    }
    if (!y_mod) {
        if (HAS_FLAG(input_flags, UP)) { SET_FLAG(update_flags, DOWN); }
        else if (HAS_FLAG(input_flags, DOWN)) { SET_FLAG(update_flags, UP); }
    }

    return update_flags;
}
