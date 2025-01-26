#pragma once

#include <stdint.h>
#include <SDL2/SDL.h>
#include "global.h"
#include "obj.h"

typedef struct GraphicsContext {
    SDL_Window *win;
    SDL_Renderer *renderer;
} GraphicsContext;

extern void destroyContext(GraphicsContext *pTo_destroy);
extern GraphicsContext createContext(uint32_t w, uint32_t h, const char *title);
extern void render(SDL_Renderer *renderer, SDL_Texture ***terrain_map,
                   SDL_Rect *pTop_left_rect, Player *pPlayer);
extern void exitGraphicsMgr(GraphicsContext *pMain_context);