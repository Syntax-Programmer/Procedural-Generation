#ifndef PHYSICS_H
#define PHYSICS_H

#include <time.h>
#include <math.h>
#include <stdint.h>
#include "global_defs.h"

extern void getDeltaTime(time_t *pStart, double *pDelta_time, int *pFrame_c);
extern void randomGradient(int ix, int iy, int seed, float *pGradient_x, float *pGradient_y);
extern float dotGridGradient(int ix, int iy, float x, float y, int seed);
extern float interpolate(float a0, float a1, float w);
extern float perlin(float x, float y, int seed);
extern void FBM(float freq, int x, int y, int seed, uint8_t *pR, uint8_t *pG, uint8_t *pB);
extern void normalizeMoveDist(float vel, double delta_time, int x_comp, int y_comp,
                              float *pX_norm, float *pY_norm);

#endif // PHYSICS_H