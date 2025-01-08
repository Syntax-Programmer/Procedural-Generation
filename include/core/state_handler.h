#ifndef STATE_HANDLER_H
#define STATE_HANDLER_H

#include "utils/physics.h"
#include "utils/obj.h"
#include "utils/map_handler.h"

extern void handleState(Player *pPlayer, ChunkData ***pTerrain_map, int *pX_comp, int *pY_comp,
                        float *pAccumulated_x_offset, float *pAccumulated_y_offset, double delta_time,
                        int seed);

#endif // STATE_HANDLER_H