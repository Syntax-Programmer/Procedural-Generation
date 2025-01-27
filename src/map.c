#include "map.h"

static void editTexture(SDL_Texture *texture, int32_t scrn_x,
                        int32_t scrn_y, time_t seed, float freq);
static uint8_t ifGenNewTextures(SDL_Rect *pOrigin_rect);

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

SDL_Texture ***initTerrainMap(SDL_Renderer *renderer, SDL_Rect *pOrigin_rect,
                              time_t seed) {
    SDL_Texture ***terrain_map = malloc(ROW_C * sizeof(SDL_Texture **));

    if (!terrain_map) {
        fprintf(stderr, "Failed to allocate mem for terrain map.\n");
        return NULL;
    }
    *pOrigin_rect = (SDL_Rect){.x = -CHUNK_SIZE, .y = -CHUNK_SIZE, .w = CHUNK_SIZE, .h = CHUNK_SIZE};
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

static uint8_t ifGenNewTextures(SDL_Rect *pOrigin_rect) {
    /* Could have used the input flags to figure out which 2-3 rects
    to check specifically. But this approach will introduce unneeded complexity
    and even hurt performance in certain conditions. It is easier to just check for all
    4 rects at once. */
    uint8_t update_flags = 0;
    SDL_Rect tl, tr, bl, br;
    uint8_t tl_fov, tr_fov, bl_fov, br_fov;

    tl = tr = bl = br = *pOrigin_rect;
    // We get the required rects by offsetting the coordinates of pOriginRect.
    tr.x += COL_C * CHUNK_SIZE;
    bl.y += ROW_C * CHUNK_SIZE;
    br.x = tr.x;
    br.y = bl.y;
    // FOV checks
    tl_fov = isRectInFOV(&tl);
    tr_fov = isRectInFOV(&tr);
    bl_fov = isRectInFOV(&bl);
    br_fov = isRectInFOV(&br);
    // Determining what type of updates to do.
    if (tl_fov && tr_fov && bl_fov && br_fov) { return update_flags; }
    if (!tl_fov && !tr_fov) { // Strictly down moving.
        SET_FLAG(update_flags, DOWN);
        if (!bl_fov) { SET_FLAG(update_flags, RIGHT); } // Moving downward right.
        else if (!br_fov) { SET_FLAG(update_flags, LEFT); } // Moving downward left.
    } else if (!bl_fov && !br_fov) { // Strictly up moving.
        SET_FLAG(update_flags, UP);
        if (!bl_fov) { SET_FLAG(update_flags, RIGHT); } // Moving upward right.
        else if (!br_fov) { SET_FLAG(update_flags, LEFT); } // Moving upward left.
    } else {
        if (!bl_fov) { SET_FLAG(update_flags, RIGHT); } // Moving strictly right.
        else if (!br_fov) { SET_FLAG(update_flags, LEFT); } // Moving strictly left.
    }

    return update_flags;
}

void updateTerrainMap(SDL_Texture ***terrain_map, SDL_Rect *pOrigin_rect,
                      time_t seed) {
    uint8_t update_flags = ifGenNewTextures(pOrigin_rect);

    if (!update_flags) { return; }
    if (HAS_FLAG(update_flags, UP)) {
        pOrigin_rect->y -= CHUNK_SIZE;
        for (int32_t i = 0; i < COL_C; i++) {
            if (terrain_map[ROW_C - 1][i]) { SDL_DestroyTexture(terrain_map[ROW_C - 1][i]); }
        }
        for (int32_t y = ROW_C - 2; y >= 0; y--) {
            for (int32_t x = 0; x < COL_C; x++) {
                printf("%d %d\n", y, x);
                terrain_map[y + 1][x] = terrain_map[y][x];
            }
        }
        //memmove(terrain_map[1], terrain_map[0], (ROW_C - 1) * sizeof(SDL_Texture **));
        for (int32_t i = 0; i < COL_C; i++) {
            if (terrain_map[0][i]) { editTexture(terrain_map[0][i], i * CHUNK_SIZE, pOrigin_rect->y,
                                                 seed, PERLIN_TERRAIN_FREQ); }
        }
    } else if (HAS_FLAG(update_flags, DOWN)) {

    }
    if (HAS_FLAG(update_flags, RIGHT)) {

    } else if (HAS_FLAG(update_flags, LEFT)) {

    }
}