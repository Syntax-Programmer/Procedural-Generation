#ifndef GRAPHICS_HANDLER_H
#define GRAPHICS_HANDLER_H

#include "SDL2/SDL.h"
#include "utils/obj.h"
#include "global_defs.h"
#include "utils/map_handler.h"

typedef struct Context {
    SDL_Window* win;
    SDL_Renderer* renderer;
} Context;

extern void destroyContext(Context *pTo_destroy);
extern Context createContext(const char *title, int w, int h);
extern int initGraphics();
extern void quitGraphics(Context *pMain_context);
extern void render(SDL_Renderer *renderer, Player *pPlayer, ChunkData **terrain_map,
                   float cam_x, float cam_y);
extern int handleEvents(int *pX_comp, int *pY_Comp);

#endif // GRAPHICS_HANDLER_H