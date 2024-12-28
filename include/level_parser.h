#ifndef LEVEL_PARSER_H
#define LEVEL_PARSER_H

#include <json-c/json.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include "object.h"
#include "global_consts.h"

extern void freeLvlTileset(Obj** lvl_data);
extern void parseLvl(int lvl_number, Obj*** pLvl_tileset);

#endif