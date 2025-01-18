#include "utils/mapGen.h"

Chunk createChunk(SDL_Renderer *renderer, int cam_x, int cam_y,
                  int scrn_x, int scrn_y, int seed, float freq) {
    SDL_Rect dest = {.w = CHUNK_SIZE, .h = CHUNK_SIZE, .x = scrn_x, .y = scrn_y};
    Chunk chnk = {.dest = dest, .txtr = NULL};
    uint8_t r = 0, g = 0, b = 0;
    unsigned char *pixels;
    int pitch, offset = 0, grid_x = 0, grid_y = 0;
    uint32_t color = 0;

    chnk.txtr = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
                                  CHUNK_SIZE, CHUNK_SIZE);
    if (!chnk.txtr) {
        fprintf(stderr, "Couldn't initialize texture on the screen pos: X:%d, Y:%d\n", scrn_x, scrn_y);
        return chnk; // WE only need to check for txtr == NULL for invalidity.
    }
    SDL_LockTexture(chnk.txtr, NULL, (void **)&pixels, &pitch);
    for (int y = 0; y < CHUNK_SIZE; y += TILE_SIZE) {
        for (int x = 0; x <  CHUNK_SIZE; x += TILE_SIZE) {

            grid_x = (scrn_x + x - cam_x) / TILE_SIZE;
            grid_y = (scrn_y + y - cam_y) / TILE_SIZE;
            FBM(freq, grid_x, grid_y, seed, &r, &g, &b);
            color = (255 << 24) | (r << 16) | (g << 8) | b;

            // This loop fills the entire tile size square with the calculated size of the tile.
            for (int i = 0; i < TILE_SIZE; i++) {
                for (int j = 0; j < TILE_SIZE; j++) {
                    offset = (y + i) * pitch + (x + j) * 4;
                    *(uint32_t*)&pixels[offset] = color;
                }
            }
        }
    }
    SDL_UnlockTexture(chnk.txtr);

    return chnk;
}

void freeChunkMap(Chunk **to_free) {
    if (!to_free) {
        return;
    }
    for (int y = 0; y < ROW_C && to_free[y]; y++) {
        for (int x = 0; x < COL_C && to_free[y][x].txtr; x++) {
            SDL_DestroyTexture(to_free[y][x].txtr);
            to_free[y][x].txtr = NULL;
        }
        free(to_free[y]);
        to_free[y] = NULL;
    }
    free(to_free);
}

Chunk **initTerrainMap(SDL_Renderer *renderer, int seed) {
    /* As this is a init function, cam_x and cam_y are assumed to be 0 */
    Chunk **terrain_map = malloc(ROW_C * sizeof(Chunk *));

    if (!terrain_map) {
        fprintf(stderr, "Falied to allocate mem for terrain map.\n");
        return NULL;
    }
    for (int y = -1; y < ROW_C - 1 ; y++) {
        terrain_map[y + 1] = malloc(COL_C * sizeof(Chunk));
        if (!terrain_map[y + 1]) {
            fprintf(stderr, "ERROR: Failed to allocated memory for col in the terrain map init.\n");
            freeChunkMap(terrain_map);
            return NULL;
        }
        for (int x = -1; x < COL_C - 1; x++) {
            terrain_map[y + 1][x + 1] = createChunk(renderer, 0, 0, x * CHUNK_SIZE, y * CHUNK_SIZE, seed, PERLIN_TERRAIN_FREQ);
            if (!terrain_map[y + 1][x + 1].txtr) {
                freeChunkMap(terrain_map);
                return NULL;
            }
        }
    }

    return terrain_map;
}