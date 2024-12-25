#include "graphics_handler.h"

static void handleKeyboard(int* pX_comp, int* pY_comp);

void destroyGameContext(GameContext* pTo_destroy) {
    if (pTo_destroy->renderer) {
        SDL_DestroyRenderer(pTo_destroy->renderer);
        pTo_destroy->renderer = NULL;
    }
    if (pTo_destroy->win) {
        SDL_DestroyWindow(pTo_destroy->win);
        pTo_destroy->win = NULL;
    }
}

GameContext createGameContext(char* title, int x, int y, int w, int h, Uint32 flags) {
    GameContext context = {NULL, NULL};

    context.win = SDL_CreateWindow(title, x, y, w, h, flags);
    if (!context.win) {
        fprintf(stderr, "ERROR: Couldn't create the win for the GameContext with title: %s\n", title);
        return context;
    }
    context.renderer = SDL_CreateRenderer(context.win, -1, SDL_RENDERER_PRESENTVSYNC);
    if (!context.renderer) {
        fprintf(stderr, "ERROR: Couldn't create the renderer for the GameContext with title: %s\n", title);
        destroyGameContext(&context);
        return context;
    }
    return context;
}

int initSDL() {
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        fprintf(stderr, "ERROR: Can't initialize SDL.\n");
        return 0;
    }
    return 1;
}

void quitSDL(GameContext* pMain_context) {
    destroyGameContext(pMain_context);
    SDL_Quit();
}

void renderObj(GameContext* pContext, Obj* pTo_render, int x_offset, int y_offset) {
    setRectPos(&(pTo_render->rect), pTo_render->rect.x - x_offset, pTo_render->rect.y - y_offset);
    // Tp ensure rects only in fov get rendered.
    if (!isRectInFOV(&(pTo_render->rect))) {
        return;
    }
    SDL_SetRenderDrawColor(pContext->renderer, pTo_render->r, pTo_render->g, pTo_render->b, pTo_render->a);
    SDL_RenderFillRect(pContext->renderer, &(pTo_render->rect));
}

void render(GameContext* pContext, Player* pPlayer, Obj** lvl_data, int x_offset, int y_offset) {
    SDL_SetRenderDrawColor(pContext->renderer, 255, 255, 255, 255);
    SDL_RenderClear(pContext->renderer);
    SDL_SetRenderDrawColor(pContext->renderer, pPlayer->obj.r, pPlayer->obj.g, pPlayer->obj.b, pPlayer->obj.a);
    SDL_RenderFillRect(pContext->renderer, &(pPlayer->obj.rect));
    for (int i = 0; lvl_data[i]; i++) {
        renderObj(pContext, lvl_data[i], x_offset, y_offset);
    }
    SDL_RenderPresent(pContext->renderer);
}

static void handleKeyboard(int* pX_comp, int* pY_comp) {
    const Uint8* state = SDL_GetKeyboardState(NULL);

    if (state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP]) {
        *pY_comp -= 1;
    } if (state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT]) {
        *pX_comp -= 1;
    } if (state[SDL_SCANCODE_S] || state[SDL_SCANCODE_DOWN]) {
        *pY_comp += 1;
    } if (state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT]) {
        *pX_comp += 1;
    }
}

int handleEvents(int* pX_comp, int* pY_Comp) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return 0;
        }
    }
    handleKeyboard(pX_comp, pY_Comp);
    return 1;
}