#ifndef OBJECT_H
#define OBJECT_H

#include <SDL.h>
#include "physics.h"

typedef struct Stat {
    Uint8 max, curr;
} Stat;
typedef struct Obj {
    SDL_Rect rect;
    Uint8 r, g, b, a;
} Obj;

extern Stat createStat(Uint8 max);
extern void setStatMax(Stat* pStat, Uint8 max);
extern void setStatCurr(Stat* pStat, int change);
extern Obj createObj(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

#endif