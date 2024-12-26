#ifndef LEVEL_PARSER_H
#define LEVEL_PARSER_H

#include "object.h"
#include "global_consts.h"

extern void freeLvlData(Obj** lvl_data);
extern int reallocLvlData(Obj*** pLvl_data, int prev_size);
extern Obj** parseLevel(int lvl_number);

#endif