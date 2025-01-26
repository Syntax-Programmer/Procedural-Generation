#include "map.h"

static void editTexture(SDL_Texture *texture, int32_t scrn_x,
                        int32_t scrn_y, time_t seed, float freq);

static void editTexture(SDL_Texture *texture, int32_t scrn_x,
                        int32_t scrn_y, time_t seed, float freq) {
    uint8_t r = 0, g = 0, b = 0;
    unsigned char *pixels;
    int32_t pitch = 0, offset = 0, grid_x = 0, grid_y = 0;
    uint32_t color = 0;

    SDL_LockTexture(texture, NULL, (void **)&pixels, &pitch);
    for (int y = 0; y < CHUNK_SIZE; y += TILE_SIZE) {
        for (int x = 0; x <  CHUNK_SIZE; x += TILE_SIZE) {
            grid_x = (scrn_x + x) / TILE_SIZE;
            grid_y = (scrn_y + y) / TILE_SIZE;
            FBM(freq, grid_x, grid_y, seed, &r, &g, &b);
            color = (255 << 24) | (r << 16) | (g << 8) | b;
            // This loop fills the entire tile size square with the calculated size of the tile.
            for (int i = 0; i < TILE_SIZE; i++) {
                for (int j = 0; j < TILE_SIZE; j++) {
                    i %= TILE_SIZE;
                    j %= TILE_SIZE;
                    offset = (y + i) * pitch + (x + j) * 4;
                    *(uint32_t*)&pixels[offset] = color;
                }
            }
        }
    }
    SDL_UnlockTexture(texture);
}

void freeTerrainMap(SDL_Texture ***terrain_map) {
    if (!terrain_map) { return; }
    for (int y = 0; y < ROW_C; y++) {
        if (terrain_map[y]) {
            for (int x = 0; x < COL_C; x++) {
                if (terrain_map[y][x]) {
                    SDL_DestroyTexture(terrain_map[y][x]);
                }
            }
            free(terrain_map[y]);
        }
    }
    free(terrain_map);
}

SDL_Texture ***initTerrainMap(SDL_Renderer *renderer, SDL_Rect *pTop_left_rect,
                              time_t seed) {
    SDL_Texture ***terrain_map = malloc(ROW_C * sizeof(SDL_Texture **));

    if (!terrain_map) {
        fprintf(stderr, "Failed to allocate mem for terrain map.\n");
        return NULL;
    }
    *pTop_left_rect = (SDL_Rect){.x = -CHUNK_SIZE, .y = -CHUNK_SIZE, .w = CHUNK_SIZE, .h = CHUNK_SIZE};
    for (int32_t y = -1; y < ROW_C - 1; y++) {
        terrain_map[y + 1] = malloc(COL_C * sizeof(SDL_Texture *));
        if (!terrain_map[y + 1]) {
            fprintf(stderr, "Failed to allocated memory for col in the terrain map init.\n");
            freeTerrainMap(terrain_map);
            return NULL;
        }
        for (int32_t x = -1; x < COL_C - 1; x++) {
            terrain_map[y + 1][x + 1] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
                                                          CHUNK_SIZE, CHUNK_SIZE);
            if (!terrain_map[y + 1][x + 1]) { continue; } // Ignoring uncreated textures rather than failing.
            editTexture(terrain_map[y + 1][x + 1], x * CHUNK_SIZE, y * CHUNK_SIZE, seed, PERLIN_TERRAIN_FREQ);
        }
    }

    return terrain_map;
}