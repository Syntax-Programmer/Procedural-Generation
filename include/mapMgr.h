#pragma once

#include <stdint.h>
#include <SDL2/SDL.h>
#include <time.h>
#include <string.h>
#include "global.h"
#include "physics.h"



/// @brief Initializes the terrain map for the game.
/// @param renderer The renderer the game uses.
/// @param pTerrain_map The pointer to the terrain map to initialize.
/// @param pMap_pos_handle The pointer to the position handle to initialize with the map.
/// @param seed The random seed the game uses.
extern void initTerrainMap(SDL_Renderer *renderer, SDL_Texture ***pTerrain_map,
                           PosHandle *pMap_pos_handle, time_t seed);

/// @brief Frees the initialized terrain map.
/// @param terrain_map The already initialized terrain map.
extern void freeTerrainMap(SDL_Texture **terrain_map);
