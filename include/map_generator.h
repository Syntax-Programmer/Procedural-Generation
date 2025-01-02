#ifndef MAP_GENERATOR_H
#define MAP_GENERATOR_H

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "global_consts.h"
#include "physics.h"
#include "object.h"

extern void freeTerrainMap(Obj **to_free);
extern Obj **generateTerrainMap();

#endif