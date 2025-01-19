#include "core/graphicsMgr.h"

void destroyContext(GraphicsContext *pTo_destroy) {
    if (!pTo_destroy) { return; }
    if (pTo_destroy->renderer) {
        SDL_DestroyRenderer(pTo_destroy->renderer);
        pTo_destroy->renderer = NULL;
    }
    if (pTo_destroy->win) {
        SDL_DestroyWindow(pTo_destroy->win);
        pTo_destroy->win = NULL;
    }
}

GraphicsContext createContext(int w, int h, const char *title) {
    GraphicsContext context = {.win = NULL, .renderer = NULL};

    context.win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                   w, h, SDL_WINDOW_SHOWN);
    if (!context.win) {
        fprintf(stderr, "Unable to create win for the context with title: %s\n", title);
        return context;
    }
    context.renderer = SDL_CreateRenderer(context.win, -1, SDL_RENDERER_ACCELERATED);
    if (!context.renderer) {
        fprintf(stderr, "Unable to create renderer for the context with title: %s\n", title);
        destroyContext(&context);
        return context;
    }

    return context;
}

void render(SDL_Renderer *renderer, Chunk **terrain_map, Player *pPlayer,
            int cam_x, int cam_y) {
    SDL_RenderClear(renderer);
    for (int y = 0; y < ROW_C; y++) {
        for (int x = 0; x < COL_C; x++) {
            setRectPos(&terrain_map[y][x].dest, terrain_map[y][x].dest.x - cam_x, terrain_map[y][x].dest.y - cam_y);
            SDL_RenderCopy(renderer, terrain_map[y][x].txtr, NULL, &terrain_map[y][x].dest);
            setRectPos(&terrain_map[y][x].dest, terrain_map[y][x].dest.x + cam_x, terrain_map[y][x].dest.y + cam_y);
        }
    }
    SDL_SetRenderDrawColor(renderer, pPlayer->player_obj.r, pPlayer->player_obj.g,
                           pPlayer->player_obj.b, 255);
    SDL_RenderFillRect(renderer, &pPlayer->player_obj.rect);
    SDL_RenderPresent(renderer);
}

void exitGraphicsMgr(GraphicsContext *pMain_context) {
    destroyContext(pMain_context);
    SDL_Quit();
}