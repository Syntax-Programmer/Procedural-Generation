#include "state_handler.h"

static void attemptRollbackMove(Obj** lvl_data, int rollback_limit, int x_offset, int y_offset);

static void attemptRollbackMove(Obj** lvl_data, int rollback_limit, int x_offset, int y_offset) {
    for (int i = 0; i < rollback_limit; i++) {
        setRectPos(&(lvl_data[i]->rect), lvl_data[i]->rect.x + x_offset, lvl_data[i]->rect.y + y_offset);
    }
}

extern void handleState(Player* pPlayer, Obj*** pTerrain_map, int x_comp, int y_comp,
                        int* pX_offset, int* pY_offset, double delta_time) {
    int colliding_obj_i = 0;

    getWorldMovOffset(pPlayer->vel, delta_time, x_comp, y_comp, pX_offset, pY_offset);
    for (int i = 0; (*pTerrain_map)[i]; i++) {
        setRectPos(&((*pTerrain_map)[i]->rect), (*pTerrain_map)[i]->rect.x - (*pX_offset),
                   (*pTerrain_map)[i]->rect.y - (*pY_offset)); // Moving the world relative to the player.
        // Only check for collision in rects within FOV.
        if (!isRectInFOV(&((*pTerrain_map)[i]->rect))) {
            continue;
        }
        if (pPlayer->obj.can_collide && (*pTerrain_map)[i]->can_collide && // Checking if the player and the object can collide before doing the collision check.
            AABBCollision(&(pPlayer->obj.rect), &((*pTerrain_map)[i]->rect))) {
            colliding_obj_i = i + 1;
            break;
        }
    }
    //We attempt rolling back the move if colliding_obj_i != 0, because this means that somewhere collision
    //happend and we have to move back all the rects that have already been moved.*
    attemptRollbackMove(*pTerrain_map, colliding_obj_i, *pX_offset, *pY_offset);
}