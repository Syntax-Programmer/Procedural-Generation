#ifndef MAP_MANAGER_H
#define MAP_MANAGER_H

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <SDL_rect.h>
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

extern int *initPTable();
extern int addColNode(ScreenColData **to_add, int add_to_start,
                      int x, int y, int w, int h, int can_collide, Uint32 color_hex);
extern void freeScreenData(ScreenColData **to_free);
extern ScreenColData **initScreenData(int *pP_table, float freq, int seed);
extern void findDataOutOfFOV(ScreenColData **data, int accumulated_x_offset, int accumulated_y_offset,
                             int *pX_out_of_fov, int *pY_out_of_fov);
extern int updateDataOutOfFOV(ScreenColData **data, int x_out_of_fov, int y_out_of_fov, float freq, int *pP_table, int seed);

#endif