#pragma once

#include <stdio.h>
#include <SDL2/SDL.h>
#include "global.h"

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

extern uint8_t isRectInFOV(SDL_Rect *pRect);
extern Obj createObj(int32_t x, int32_t y, int32_t w, int32_t h, Uint8 r, Uint8 g, Uint8 b);
extern void setStatBarCurr(StatBar *pBar, uint8_t to_set);
extern void setStatBarMax(StatBar *pBar, uint8_t to_set);
extern uint8_t getStatBarMax(StatBar bar);
extern uint8_t getStatBarCurr(StatBar bar);
extern Player createPlayer(int32_t x, int32_t y, int32_t w, int32_t h, Uint8 r, Uint8 g, Uint8 b,
                           uint16_t vel, uint8_t init_health);