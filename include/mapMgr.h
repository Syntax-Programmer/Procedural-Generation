#pragma once

#include <stdint.h>
#include <SDL2/SDL.h>
#include <time.h>
#include <string.h>
#include "global.h"
#include "physics.h"

/*
 * @brief Initializes the terrain_map of the game.
 *
 * Allocates the space for the terrain_map array, handles errors and
 * gives them a perlin noise texture using the given seed.
 *
 * @param renderer The SDL_Renderer from the created GraphicsContext
 *                 that the rendering will take place upon.
 * @param pTerrain_map The pointer to the terrain_map where we will
 *                     put the pointer of the initialized terrain_map.
 * @param *pWorldPosHandler Used to manage map and cam positions of the game
 *                          that will be initialized with the terrain_map as it is
 *                          pos handle of this particular map.
 * @param seed The seed for the current session of this code running used to
 *             update/handle things that were generated with or require randomness.
 */
extern void initTerrainMap(SDL_Renderer *renderer, SDL_Texture ***pTerrain_map,
                           PosHandle *pMap_pos_handle, time_t seed);

/*
 * @brief Frees up the initialized terrain_map and destroys the SDL_Texture it has.
 *
 * @param terrain_map The terrain_map that is to be freed.
 */
extern void freeTerrainMap(SDL_Texture **terrain_map);
