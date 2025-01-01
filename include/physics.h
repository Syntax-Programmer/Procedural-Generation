#ifndef PHYSICS_H
#define PHYSICS_H

#include <SDL.h>
#include <math.h>
#include <time.h>
#include "global_consts.h"

typedef struct Vector2D {
    float x;
    float y;
} Vector2D;

extern void getDeltaTime(time_t* pStart, double* pDelta_time, int* pFrame_c);
extern SDL_Rect createRect(int x, int y, int w, int h);
extern void setRectPos(SDL_Rect* pRect, int x, int y);
extern void setRectSize(SDL_Rect* pRect, int w, int h);
extern void getWorldMovOffset(int vel, double delta_time, int x_comp, int y_comp, int* pX_norm, int* pY_norm);
extern int AABBCollision(const SDL_Rect* pRect1, const SDL_Rect* pRect2);
extern int isRectInFOV(const SDL_Rect* pRect);
extern Vector2D createVector2D(float x, float y);
extern int vectorDotProduct(Vector2D* pVec1, Vector2D* pVec2);

#endif