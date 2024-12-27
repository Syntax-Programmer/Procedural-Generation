#include "state_handler.h"


extern void handleState(Player* pPlayer, Obj*** lvl_data, int x_comp, int y_comp,
                        int* pX_offset, int* pY_offset, double delta_time) {
    int colliding_obj_i = 0;

    getWorldMovOffset(pPlayer->vel, delta_time, x_comp, y_comp, pX_offset, pY_offset);
    for (int i = 0; (*lvl_data)[i]; i++) {
        setRectPos(&((*lvl_data)[i]->rect), (*lvl_data)[i]->rect.x - (*pX_offset), (*lvl_data)[i]->rect.y - (*pY_offset)); // Moving the world relative to the player.
        // Only check for collision in rects within FOV.
        if (!isRectInFOV(&((*lvl_data)[i]->rect))) {
            continue;
        }
        if (AABBCollision(&(pPlayer->obj.rect), &((*lvl_data)[i]->rect))) {
            colliding_obj_i = i + 1;
            break;
        }
    }
    for (int i = 0; i < colliding_obj_i; i++) {
        setRectPos(&((*lvl_data)[i]->rect), (*lvl_data)[i]->rect.x + (*pX_offset), (*lvl_data)[i]->rect.y + (*pY_offset)); // Moving back only the objs that were moved if collided.
    }
}