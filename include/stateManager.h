#pragma once

#include <stdint.h>
#include <SDL2/SDL.h>
#include <time.h>
#include "physics.h"
#include "obj.h"

extern void handleState(uint16_t input_flags, time_t seed, SDL_Rect *pTop_left_Rect,
                SDL_Texture ***terrain_map, Player *pPlayer, double delta_time);