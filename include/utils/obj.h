#ifndef OBJ_H
#define OBJ_H

#include "SDL2/SDL_rect.h"

typedef struct Obj {
    SDL_FRect rect;
    uint8_t r, g, b;
} Obj;
typedef struct Player {
    Obj obj;
    float vel;
} Player;

extern Obj createObj(float x, float y, float w, float h, uint8_t r, uint8_t g, uint8_t b);
extern void setObjPos(Obj *pTo_update, float x, float y);
extern void setObjSize(Obj *pTo_update, float w, float h);
extern void setRectPos(SDL_FRect *pTo_update, float x, float y);
extern void setRectSize(SDL_FRect *pTo_update, float w, float h);
extern int isObjColliding(Obj *pObj1, Obj *pObj2);
extern Player createPlayer(float x, float y, float w, float h, uint8_t r, uint8_t g, uint8_t b,
                           float vel);

#endif // OBJ_H