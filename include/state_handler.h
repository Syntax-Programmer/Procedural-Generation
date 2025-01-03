#ifndef STATE_HANDLER_H
#define STATE_HANDLER_H

#include "global_consts.h"
#include "player.h"
#include "physics.h"
#include "map_generator.h"

extern void handleState(Player *pPlayer, int *pX_comp, int *pY_comp,
                        int *pAccumulated_x_offset, int *pAccumulated_y_offset, double delta_time);

#endif