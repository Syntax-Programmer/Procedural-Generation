#include "core/stateMgr.h"

void handleState(SDL_Renderer *renderer, Chunk **terrain_map, Player *pPlayer,
                 double delta_time, uint16_t input_flags, int seed, int *pCam_x, int *pCam_y) {
    int world_x = 0, world_y = 0;

    normalizeMoveDist(pPlayer->vel, delta_time, input_flags, &world_x, &world_y);
    *pCam_x += world_x;
    *pCam_y += world_y;
}