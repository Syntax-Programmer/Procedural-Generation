#pragma once

#include <SDL2/SDL.h>
#include "utils/globals.h"

#define LOWER_MASK 0x00FF
#define UPPER_MASK 0xFF00

typedef uint16_t StatBar; // Its like (max << 8) | curr
typedef struct Obj {
    SDL_Rect rect;
    Uint8 r, g, b;
} Obj;
typedef struct Player {
    Obj player_obj;
    StatBar health;
    uint16_t vel;
} Player;

extern void setRectPos(SDL_Rect *pRect, int x, int y);
extern int isRectInFOV(SDL_Rect *pRect);
extern Obj createObj(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b);
extern void setStatBarCurr(StatBar *pBar, uint8_t to_set);
extern void setStatBarMax(StatBar *pBar, uint8_t to_set);
extern uint8_t getStatBarMax(StatBar bar);
extern uint8_t getStatBarCurr(StatBar bar);
extern Player createPlayer(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b,
                           uint16_t vel, uint8_t init_health);