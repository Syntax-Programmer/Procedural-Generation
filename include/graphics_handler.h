#ifndef GRAPHICS_HANDLER_H
#define GRAPHICS_HANDLER_H

#include <SDL.h>
#include "physics.h"
#include "player.h"
#include "global_consts.h"

typedef struct GameContext {
    SDL_Window* win;
    SDL_Renderer* renderer;
} GameContext;

extern void destroyGameContext(GameContext* pTo_destroy);
extern GameContext createGameContext(char* title, int x, int y, int w, int h, Uint32 flags);
extern int initSDL();
extern void quitSDL(GameContext* pMain_context);
extern void renderObj(GameContext* pContext, Obj* pTo_render, int x_offset, int y_offset);
extern void render(GameContext* pContext, Player* pPlayer, Obj** lvl_data, int x_offset, int y_offset);
extern int handleEvents(int* pX_comp, int* pY_Comp);

#endif