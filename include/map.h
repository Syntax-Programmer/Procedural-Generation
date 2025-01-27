#pragma once

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <string.h>
#include "physics.h"
#include "global.h"
#include "obj.h"

extern void freeTerrainMap(SDL_Texture ***terrain_map);
extern SDL_Texture ***initTerrainMap(SDL_Renderer *renderer, SDL_Rect *pOrigin_rect,
                                     time_t seed);
extern void updateTerrainMap(SDL_Texture ***terrain_map, SDL_Rect *pOrigin_rect,
                             time_t seed);