#include "core/state_handler.h"

void handleState(Player *pPlayer, ChunkData ***pTerrain_map, SDL_Renderer *renderer,
                 int *pX_comp, int *pY_comp, float *pCam_x, float *pCam_y,
                 double delta_time, int seed) {
    float world_x_offset = 0, world_y_offset = 0;

    normalizeMoveDist(pPlayer->vel, delta_time, *pX_comp, *pY_comp,
                      &world_x_offset, &world_y_offset);
    if (world_x_offset || world_y_offset) {
        for (int y = 0; y < ROW_CHUNK_COUNT; y++) {
            for (int x = 0; x < COL_CHUNK_COUNT; x++) {
                setRectPos(&(*pTerrain_map)[y][x].dest, (*pTerrain_map)[y][x].dest.x - world_x_offset, (*pTerrain_map)[y][x].dest.y - world_y_offset);
            }
        }
    }
    // Racking up what the player moved.
    *pCam_x += world_x_offset;
    *pCam_y += world_y_offset;
    /*
    if (!updateTextureMap(*pTerrain_map, renderer, *pCam_x, *pCam_y, seed, PERLIN_TERRAIN_FREQ)) {
        *pTerrain_map = NULL;
        assert("ERROR");
    }*/
    *pX_comp = *pY_comp = 0;
}