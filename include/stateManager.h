#pragma once

#include <stdint.h>
#include <SDL2/SDL.h>
#include <time.h>
#include "physics.h"
#include "obj.h"
#include "map.h"

extern void handleState(uint16_t input_flags, time_t seed, SDL_Rect *pOrigin_rect,
                SDL_Texture ***terrain_map, Player *pPlayer, double delta_time);