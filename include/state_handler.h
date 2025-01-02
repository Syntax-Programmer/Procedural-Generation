#ifndef STATE_HANDLER_H
#define STATE_HANDLER_H

#include "global_consts.h"
#include "player.h"
#include "physics.h"
#include "object.h"

extern void handleState(Player *pPlayer, Obj ***pTerrain_map, int x_comp, int y_comp,
                        int *pX_offset, int *pY_offset, double delta_time);

#endif