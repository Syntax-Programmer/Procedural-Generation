#pragma once

#include <time.h>
#include <math.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include "utils/globals.h"

#define INTERPOLATE(a1, a2, w) ((a2 - a1) * ((6 * w - 15) * w + 10) * w * w * w + a1)

extern void getDeltaTime(Uint32 *pStart, double *pDelta_time, int *pFrame_c);
extern void randomGradient(int ix, int iy, int seed, float *pGradient_x, float *pGradient_y);
extern float dotGridGradient(int ix, int iy, float x, float y, int seed);
extern float perlin(float x, float y, int seed);
extern void FBM(float freq, int x, int y, int seed, uint8_t *pR, uint8_t *pG, uint8_t *pB);
extern void normalizeMoveDist(int vel, double delta_time, uint16_t input_flags, int *pX_norm, int *pY_norm);