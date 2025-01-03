#ifndef MAP_GENERATOR_H
#define MAP_GENERATOR_H

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "global_consts.h"
#include "physics.h"
#include "object.h"

typedef struct ScreenColData ScreenColData;
struct ScreenColData
{
    Obj obj;
    ScreenColData *prev, *next;
}; // A doubly circular linked list.
//? ScreenColData[SCREEN_HEIGHT / TILE_SQUARE_SIZE] is the entire screen data.

int addColNode(ScreenColData **to_add, int add_to_start,
               int x, int y, int w, int h, int can_collide, Uint32 color_hex);
void freeScreenData(ScreenColData **to_free);
ScreenColData **initScreenData();

#endif