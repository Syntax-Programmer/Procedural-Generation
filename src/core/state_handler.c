#include "core/state_handler.h"

void handleState(Player *pPlayer, ChunkData ***pTerrain_map, int *pX_comp, int *pY_comp,
                 float *pAccumulated_x_offset, float *pAccumulated_y_offset, double delta_time,
                 int seed) {
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
    *pAccumulated_x_offset += world_x_offset;
    *pAccumulated_y_offset += world_y_offset;
    *pX_comp = *pY_comp = 0;
}