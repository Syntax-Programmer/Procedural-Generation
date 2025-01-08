#include "utils/obj.h"

Obj createObj(float x, float y, float w, float h, uint8_t r, uint8_t g, uint8_t b) {
    return (Obj){.r = r, .g = g, .b = b,
                 .rect = {.x = x, .y = y, .w = w, .h = h}};
}

void setObjPos(Obj *pTo_update, float x, float y) {
    pTo_update->rect.x = x;
    pTo_update->rect.y = y;
}

void setObjSize(Obj *pTo_update, float w, float h) {
    pTo_update->rect.w = w;
    pTo_update->rect.h = h;
}

void setRectPos(SDL_FRect *pTo_update, float x, float y) {
    pTo_update->x = x;
    pTo_update->y = y;
}

void setRectSize(SDL_FRect *pTo_update, float w, float h) {
    pTo_update->w = w;
    pTo_update->h = h;
}

int isObjColliding(Obj *pObj1, Obj *pObj2) {
    return (
        pObj1->rect.x < pObj2->rect.x + pObj2->rect.w &&
        pObj1->rect.x + pObj1->rect.w > pObj2->rect.x &&
        pObj1->rect.y < pObj2->rect.y + pObj2->rect.h &&
        pObj1->rect.y + pObj1->rect.h > pObj2->rect.y);
}

Player createPlayer(float x, float y, float w, float h, uint8_t r, uint8_t g, uint8_t b,
                    float vel) {
    return (Player){.vel = vel, .obj = createObj(x, y, w, h, r, g, b)};
}