#include "utils/obj.h"

void setRectPos(SDL_Rect *pRect, int x, int y) {
    pRect->x = x;
    pRect->y = y;
}

int isRectInFOV(SDL_Rect *pRect) {
    return 0;
}

Obj createObj(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b) {
    Obj object = {.r = r, .g = g, .b = b};

    object.rect = (SDL_Rect){.x = x, .y = y, .w = w, .h = h};

    return object;
}
void setStatBarCurr(StatBar *pBar, uint8_t to_set) {
    uint8_t max_value = (*pBar & UPPER_MASK) >> 8;

    if (to_set > max_value) {
        to_set = max_value;
    }
    *pBar = (*pBar & UPPER_MASK) | to_set;
}

void setStatBarMax(StatBar *pBar, uint8_t to_set) {
    uint8_t curr_value = *pBar & LOWER_MASK;

    if (!to_set) {
        fprintf(stderr, "Unable to set stat bar max to 0.\n");
        return;
    }
    if (curr_value > to_set) {
        setStatBarCurr(pBar, to_set);
    }
    *pBar = (*pBar & LOWER_MASK) | (to_set << 8);
}

uint8_t getStatBarMax(StatBar bar) {
    return (bar & UPPER_MASK) >> 8;
}

uint8_t getStatBarCurr(StatBar bar) {
    return (bar & LOWER_MASK);
}

Player createPlayer(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b,
                    uint16_t vel, uint8_t init_health) {
    Player player = {.vel = vel, .health = 0};

    setStatBarMax(&player.health, init_health);
    setStatBarCurr(&player.health, init_health);
    player.player_obj = createObj(x, y, w, h, r, g, b);

    return player;
}