#pragma once

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "physics.h"
#include "global.h"

extern void freeTerrainMap(SDL_Texture ***terrain_map);
extern SDL_Texture ***initTerrainMap(SDL_Renderer *renderer, SDL_Rect *pTop_left_rect,
                                     time_t seed);