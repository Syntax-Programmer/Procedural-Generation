#ifndef STATE_HANDLER_H
#define STATE_HANDLER_H

#include "global_consts.h"
#include "player.h"
#include "physics.h"
#include "object.h"

extern void manageState(Player* pPlayer, Obj*** lvl_data, int x_comp, int y_comp,
                        int* pX_offset, int* pY_offset);

#endif