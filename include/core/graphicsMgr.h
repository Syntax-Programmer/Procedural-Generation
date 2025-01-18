#pragma once

#include <SDL2/SDL.h>
#include "utils/globals.h"
#include "utils/mapGen.h"
#include "utils/obj.h"

typedef struct GraphicsContext {
    SDL_Window *win;
    SDL_Renderer *renderer;
} GraphicsContext;

extern void destroyContext(GraphicsContext *pTo_destroy);
extern GraphicsContext createContext(int w, int h, const char *title);
extern void render(SDL_Renderer *renderer, Chunk **terrain_map, Player *pPlayer,
                   int cam_x, int cam_y);
extern void exitGraphicsMgr(GraphicsContext *pMain_context);