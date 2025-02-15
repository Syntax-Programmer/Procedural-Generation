#pragma once

#include <math.h>
#include <time.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include "global.h"

#define INTERPOLATE(a1, a2, w) ((a2 - a1) * ((6 * w - 15) * w + 10) * w * w * w + a1)

/*
 * @brief A 2-D integer vector.
 */
typedef struct IntVec2 {
    int32_t x, y;
} IntVec2;

/*
 * @brief A 2-D float vector.
 */
typedef struct FloatVec2 {
    float x, y;
} FloatVec2;

/*
 * Has the data of map and cam positions.
 *
 * @elem origin_chunk_pos The global position of the top left most chunk in the maps.
 * @elem cam_pos Initially at 0, this is the camera position as the player moves.
 *
 * So with this logic, screen position of a chunk is:
 * global_coord - cam_coords.
 */
typedef struct PosHandle {
    IntVec2 origin_chunk_pos;
    IntVec2 cam_pos;
} PosHandle;

/*
 * @brief Computes the average delta time(seconds) b/w each frame.
 *
 * Using **SDL_GetTicks()** and converting the result from ms to s.
 *
 * @param pStart The pointer to the starting time to base our calculations
 *               upon, initialized at the gameloop's starting.
 * @param pDelta_time The pointer to the double that holds the calculated delta time.
 * @param pFrame_c The pointer the int that counts how many frames have been elapsed
 *                 since the last updated delta_time output.
 */
extern void getDeltaTime(Uint32 *pStart, double *pDelta_time, uint8_t *pFrame_c);

/*
 * @brief Computes a random color using perlin noise.
 *
 * Calculates a color using perlin noise and then refines it using
 * Fractal Brownian Motion to generate a random value, that is then
 * interpreted into a color.
 *
 * @param freq The frequency that the noise function will work on.
 * @param pGrid_pos The pointer to the grid pos of the point to calculate
 *                  the noise color for.
 * @param seed The seed for the current session of this code running used to
 *             update/handle things that were generated with or require randomness.
 *
 * @return The color encoded as rrrrrrrrggggggggbbbbbbbbaaaaaaaa in a uint32_t.
 */
extern uint32_t FBM(float freq, IntVec2 *pGrid_pos, time_t seed);

/*
 * @brief Computes the delta dist something moves with the give vel.
 *
 * Normalizes the move vectors and find out how much a player can move per
 * frame.
 *
 * @param vel The velocity of the something that is moving.
 * @param delta_time The average delta time between each frame.
 * @param input_flags Bitmask of the inputs done by the user used to
 *                    determine what the player wants to do.
 *
 * @return A distance vector having how much to move in each x and y direction.
 */
extern IntVec2 computeMoveDist(int16_t vel, double delta_time, uint16_t input_flags);
