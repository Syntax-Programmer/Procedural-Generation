#include "stateManager.h"

void handleState(uint16_t input_flags, time_t seed, SDL_Rect *pOrigin_rect,
                SDL_Texture ***terrain_map, Player *pPlayer, double delta_time) {
    int8_t dx = 0, dy = 0;

    if (input_flags) { normalizeMoveDist(pPlayer->vel, delta_time, input_flags, &dx, &dy); }
    if (dx || dy) {
        pOrigin_rect->x -= dx;
        pOrigin_rect->y -= dy;
        updateTerrainMap(terrain_map, pOrigin_rect, seed);
    }
}