#ifndef STATE_HANDLER_H
#define STATE_HANDLER_H

#include <assert.h>
#include "utils/physics.h"
#include "utils/obj.h"
#include "utils/map_handler.h"

extern void handleState(Player *pPlayer, ChunkData ***pTerrain_map, SDL_Renderer *renderer,
                        int *pX_comp, int *pY_comp, float *pCam_x, float *pCam_y,
                        double delta_time, int seed);

#endif // STATE_HANDLER_H