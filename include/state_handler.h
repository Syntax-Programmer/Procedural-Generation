#ifndef STATE_HANDLER_H
#define STATE_HANDLER_H

#include <assert.h>
#include "global_consts.h"
#include "player.h"
#include "physics.h"
#include "map_manager.h"

extern void handleState(Player *pPlayer, ScreenColData **terrain_map, int *pP_table, int *pX_comp, int *pY_comp,
                        int *pAccumulated_x_offset, int *pAccumulated_y_offset, double delta_time, int seed);

#endif