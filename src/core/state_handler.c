#include "state_handler.h"

static void attemptRollbackMove(Obj **terrain_map, int rollback_limit, int *pX_offset, int *pY_offset);

static void attemptRollbackMove(Obj **terrain_map, int rollback_limit, int *pX_offset, int *pY_offset)
{
    for (int i = 0; i < rollback_limit; i++)
    {
        setRectPos(&(terrain_map[i]->rect), terrain_map[i]->rect.x + *pX_offset, terrain_map[i]->rect.y + *pY_offset);
    }
    if (rollback_limit)
    {
        *pX_offset = *pY_offset = 0;
    }
}

void handleState(Player *pPlayer, int *pX_comp, int *pY_comp,
                 int *pAccumulated_x_offset, int *pAccumulated_y_offset, double delta_time)
{
    int colliding_obj_i = 0, world_x_offset = 0, world_y_offset = 0;

    getWorldMovOffset(pPlayer->vel, delta_time, *pX_comp, *pY_comp, &world_x_offset, &world_y_offset);
    // Racking up what the player moved.
    *pAccumulated_x_offset += world_x_offset;
    *pAccumulated_y_offset += world_y_offset;
    *pX_comp = *pY_comp = 0;
}