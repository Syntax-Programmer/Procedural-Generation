#pragma once

#include <math.h>
#include <time.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include "global.h"

#define INTERPOLATE(a1, a2, w) ((a2 - a1) * ((6 * w - 15) * w + 10) * w * w * w + a1)

/// @brief A vector 2d.
typedef struct Vec2 {
    int32_t x, y;
} Vec2;

/// @brief Has data of position of the map and the cam.
typedef struct PosHandle {
    int32_t origin_chunk_x, origin_chunk_y; // The absolute global coords of the top-left most chunk.
    int32_t cam_x, cam_y; // The position of the camera.
    // So, considering origin_coord to be top-left coord, screen_coord = origin_coord - cam_coord.
} PosHandle;

/// @brief Computes the avg delta time each frame takes to complete.
/// @param pStart A starting time to base our calculations upon.
/// @param pDelta_time The pointer to the delta time storage variable.
/// @param pFrame_c The pointer to the counter of number of frames since our last outputted result.
extern void getDeltaTime(Uint32 *pStart, double *pDelta_time, uint8_t *pFrame_c);

/// @brief Computes the random perlin noise color based on the given parameters.
/// @param freq The frequency to operate the calculation on.
/// @param x The x pos of the tile to calculate color for.
/// @param y The y pos of the tile to calculate color for.
/// @param seed The random seed set at the program starting.
/// @return The color encoded in a int as rrrrrrrrggggggggbbbbbbbbaaaaaaaa.
extern uint32_t FBM(float freq, int32_t x, int32_t y, time_t seed);

/// @brief Normalizes and finds the delta dist a particle with given vel can move.
/// @param vel The vel of the particle to calculate the dist.
/// @param delta_time The avg delta time b/w each frame.
/// @param input_flags The input flags are bit flags that tell how the particle is moving.
/// @return A distance vector for the particle to move to.
extern Vec2 computeMoveDist(int16_t vel, double delta_time, uint16_t input_flags);
