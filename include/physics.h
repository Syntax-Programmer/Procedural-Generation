#ifndef PHYSICS_H
#define PHYSICS_H

#include <SDL.h>
#include <math.h>
#include <time.h>
#include "global_consts.h"

typedef struct
{
    float x, y;
} vector2;

extern void getDeltaTime(time_t *pStart, double *pDelta_time, int *pFrame_c);
extern SDL_Rect createRect(int x, int y, int w, int h);
extern void setRectPos(SDL_Rect *pRect, int x, int y);
extern void setRectSize(SDL_Rect *pRect, int w, int h);
extern void getWorldMovOffset(int vel, double delta_time, int x_comp, int y_comp, int *pX_norm, int *pY_norm);
extern int AABBCollision(const SDL_Rect *pRect1, const SDL_Rect *pRect2);
extern int isRectInFOV(const SDL_Rect *pRect);
extern vector2 randomGradient(int ix, int iy, int seed);
extern float dotGridGradient(int ix, int iy, float x, float y, int seed);
extern float interpolate(float a0, float a1, float w);

#endif