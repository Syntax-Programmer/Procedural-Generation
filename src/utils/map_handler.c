#include "utils/map_handler.h"
#include <vulkan/vulkan.h>

static int isRectInFOV(SDL_FRect *to_check);
static uint8_t checkTextureMapUpdate(ChunkData **texture_map, float cam_x, float cam_y);

ChunkData createTextureChunk(SDL_Renderer *renderer, float start_x, float start_y, int seed, float freq) {
    SDL_FRect temp;
    setRectSize(&temp, TILE_SQUARE_SIDE, TILE_SQUARE_SIDE);
    uint8_t r, g, b, prev_r = 0, prev_g = 0, prev_b = 0;
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
            setRectPos(&temp, x, y);
            FBM(freq, (start_x + x) / TILE_SQUARE_SIDE, (start_y + y) / TILE_SQUARE_SIDE, seed, &r, &g, &b);
            if (r != prev_r || g != prev_g || b != prev_b) { // lazy loading.
                SDL_SetRenderDrawColor(renderer, r, g, b, A);
                prev_r = r;
                prev_g = g;
                prev_b = b;
            }
            /* PERFORMACE OPTIMIZATION TODO: Make so that all the rect in a col that are of the same color
            form a big rectange that can be rendered at once. */
            SDL_RenderFillRectF(renderer, &temp);
        }
    }
    SDL_SetRenderTarget(renderer, NULL);
    data.texture = texture;
    return data;
}

void freeTerrainTextureMap(ChunkData **to_free) {
    if (!to_free) {
        return;
    }
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

static int isRectInFOV(SDL_FRect *to_check) {
    if (to_check->x >= -CHUNKSIZE && to_check->x <= SCREEN_WIDTH
        && to_check->y >= -CHUNKSIZE && to_check->y <= SCREEN_HEIGHT) {
        return 1;
    }
    return 0;
}

static uint8_t checkTextureMapUpdate(ChunkData **texture_map, float cam_x, float cam_y) {
    /* Checks if we need to update the texture map.*/
    SDL_FRect tl = texture_map[0][0].dest, tr = texture_map[0][COL_CHUNK_COUNT - 1].dest,
              bl = texture_map[ROW_CHUNK_COUNT - 1][0].dest,
              br = texture_map[ROW_CHUNK_COUNT - 1][COL_CHUNK_COUNT -1].dest;

    // This brings the rect down to, as they will be visible to the player.
    setRectPos(&tr, tr.x - cam_x, tr.y - cam_y);
    setRectPos(&tl, tl.x - cam_x, tl.y - cam_y);
    setRectPos(&br, br.x - cam_x, br.y - cam_y);
    setRectPos(&bl, bl.x - cam_x, bl.y - cam_y);

    int tl_in_fov = isRectInFOV(&tl), tr_in_fov = isRectInFOV(&tr),
        bl_in_fov = isRectInFOV(&bl), br_in_fov = isRectInFOV(&br);
    uint8_t move_flags = MOVING_NOWHERE;

    /* NOTE: This particular order of if-elseif should be maintained and not changed
    This is because the above ones have more constraints than the lower ones, so we need
    to check for them first.*/
    if (!tr_in_fov && !tl_in_fov && !br_in_fov) {
        move_flags |= MOVING_LEFT | MOVING_DOWN;
    }
    else if (!bl_in_fov && !tl_in_fov && !br_in_fov) {
        move_flags |= MOVING_RIGHT | MOVING_UP;
    }
    else if (!tl_in_fov) {
        if (!tr_in_fov) {
            move_flags |= MOVING_DOWN;
        }
        if (!bl_in_fov) {
            move_flags |= MOVING_RIGHT;
        }
    }
    else if (!br_in_fov) {
        if (!bl_in_fov) {
            move_flags |= MOVING_UP;
        }
        if (!tr_in_fov) {
            move_flags |= MOVING_LEFT;
        }
    }
    return move_flags;
}

int updateTextureMap(ChunkData **texture_map, SDL_Renderer *renderer, float cam_x,
                     float cam_y, int seed, float freq) {
    uint8_t move_flags = checkTextureMapUpdate(texture_map, cam_x, cam_y);

    if (!move_flags) {
        return 0;
    }
    if (move_flags & MOVING_UP) {
        for (int i = 0; i < COL_CHUNK_COUNT; i++) {
            SDL_DestroyTexture(texture_map[ROW_CHUNK_COUNT - 1][i].texture);
        }
        memmove(&texture_map[1], &texture_map[0], (ROW_CHUNK_COUNT - 1) * sizeof(ChunkData *));
        for (int i = 0; i < COL_CHUNK_COUNT; i++) {
            texture_map[0][i] = createTextureChunk(renderer, texture_map[1][i].dest.x,
                                                   texture_map[1][i].dest.y - CHUNKSIZE, seed, freq);
            if (!texture_map[0][i].texture) {
                freeTerrainTextureMap(texture_map);
            }
        }
    } else if (move_flags & MOVING_DOWN) {

    }
    if (move_flags & MOVING_RIGHT) {

    } else if (move_flags & MOVING_LEFT) {

    }
    return 0;
}