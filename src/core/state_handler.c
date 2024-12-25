#include "state_handler.h"


extern void handleState(Player* pPlayer, Obj*** lvl_data, int x_comp, int y_comp,
                        int* pX_offset, int* pY_offset, double delta_time) {
    int collided = 0;

    getWorldMovOffset(pPlayer->vel, delta_time, x_comp, y_comp, pX_offset, pY_offset);
    for (int i = 0; (*lvl_data)[i]; i++) {
        // Only check for collision in rects within FOV.
        if (!isRectInFOV(&((*lvl_data)[i]->rect))) {
            continue;
        }
        if (AABBCollision(&(pPlayer->obj.rect), &((*lvl_data)[i]->rect))) {
            collided = 1;
            break;
        }
    }
    pPlayer->obj.b = (collided) ? 100 : 255;
}