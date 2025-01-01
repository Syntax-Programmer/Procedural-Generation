#ifndef OBJECT_H
#define OBJECT_H

#include <SDL.h>
#include "physics.h"

typedef struct Stat {
    Uint8 max, curr;
} Stat;
typedef struct Obj {
    SDL_Rect rect;
    Uint32 color_hex;
    int can_collide;
} Obj;

extern Stat createStat(Uint8 max);
extern void setStatMax(Stat* pStat, Uint8 max);
extern void setStatCurr(Stat* pStat, int change);
extern Uint32 encodeColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
extern void decodeColor(Uint32 color_hex, Uint8* pR, Uint8* pG, Uint8* pB, Uint8* pA);
extern Obj createObj(int x, int y, int w, int h, Uint32 color_hex, int can_collide);

#endif