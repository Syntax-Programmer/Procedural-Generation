#pragma once

#include "utils/mapGen.h"
#include "utils/globals.h"
#include "utils/obj.h"
#include "utils/phy.h"

extern void handleState(SDL_Renderer *renderer, Chunk **terrain_map, Player *pPlayer,
                        double delta_time, uint16_t input_flags, int seed, int *pCam_x, int *pCam_y);