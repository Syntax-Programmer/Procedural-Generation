#pragma once

#include <math.h>
#include <time.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include "global.h"

#define INTERPOLATE(a1, a2, w) ((a2 - a1) * ((6 * w - 15) * w + 10) * w * w * w + a1)

extern void getDeltaTime(Uint32 *pStart, double *pDelta_time, uint8_t *pFrame_c);
extern void randomGradient(int32_t ix, int32_t iy, time_t seed,
                           float *pGradient_x, float *pGradient_y);
extern float dotGridGradient(int32_t ix, int32_t iy, float x, float y, time_t seed);
extern float perlin(float x, float y, time_t seed);
extern void FBM(float freq, int32_t x, int32_t y, time_t seed,
                uint8_t *pR, uint8_t *pG, uint8_t *pB);
extern void normalizeMoveDist(int16_t vel, double delta_time, uint16_t input_flags,
                              int8_t *pX_norm, int8_t *pY_norm);