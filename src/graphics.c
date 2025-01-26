#include "graphics.h"

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

GraphicsContext createContext(uint32_t w, uint32_t h, const char *title) {
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

void render(SDL_Renderer *renderer, SDL_Texture ***terrain_map,
            SDL_Rect *pTop_left_rect, Player *pPlayer) {
    SDL_Rect temp = *pTop_left_rect;

    SDL_RenderClear(renderer);
    for (int32_t y = 0; y < ROW_C; y++) {
        for (int32_t x = 0; x < COL_C; x++) {
            temp.x += CHUNK_SIZE;
            if (terrain_map[y][x]) {
                SDL_RenderCopy(renderer, terrain_map[y][x], NULL, &temp);
            }
        }
        temp.y += CHUNK_SIZE;
        temp.x = pTop_left_rect->x;
    }
    SDL_SetRenderDrawColor(renderer, pPlayer->player_obj.r, pPlayer->player_obj.g, pPlayer->player_obj.b, 255);
    SDL_RenderFillRect(renderer, &pPlayer->player_obj.rect);
    SDL_RenderPresent(renderer);
}

void exitGraphicsMgr(GraphicsContext *pMain_context) {
    destroyContext(pMain_context);
    SDL_Quit();
}