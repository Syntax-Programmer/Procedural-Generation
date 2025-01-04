#include "graphics_handler.h"

static void handleKeyboard(int *pX_comp, int *pY_comp);

void destroyGameContext(GameContext *pTo_destroy)
{
    if (pTo_destroy->renderer)
    {
        SDL_DestroyRenderer(pTo_destroy->renderer);
        pTo_destroy->renderer = NULL;
    }
    if (pTo_destroy->win)
    {
        SDL_DestroyWindow(pTo_destroy->win);
        pTo_destroy->win = NULL;
    }
}

GameContext createGameContext(char *title, int x, int y, int w, int h, Uint32 flags)
{
    GameContext context = {NULL, NULL};

    context.win = SDL_CreateWindow(title, x, y, w, h, flags);
    if (!context.win)
    {
        fprintf(stderr, "ERROR: Couldn't create the win for the GameContext with title: %s\n", title);
        return context;
    }
    context.renderer = SDL_CreateRenderer(context.win, -1, SDL_RENDERER_PRESENTVSYNC);
    if (!context.renderer)
    {
        fprintf(stderr, "ERROR: Couldn't create the renderer for the GameContext with title: %s\n", title);
        destroyGameContext(&context);
        return context;
    }
    return context;
}

int initSDL()
{
    if (SDL_Init(SDL_INIT_EVERYTHING))
    {
        fprintf(stderr, "ERROR: Can't initialize SDL.\n");
        return 0;
    }
    return 1;
}

void quitSDL(GameContext *pMain_context)
{
    destroyGameContext(pMain_context);
    SDL_Quit();
}

void renderObj(SDL_Renderer *renderer, Obj *pTo_render,
               int accumulated_x_offset, int accumulated_y_offset)
{
    Uint8 r, g, b, a;
    Obj new = *pTo_render;

    new.rect.x -= accumulated_x_offset;
    new.rect.y -= accumulated_y_offset;
    decodeColor(pTo_render->color_hex, &r, &g, &b, &a);
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderFillRect(renderer, &new.rect);
}

void render(GameContext *pContext, Player *pPlayer, ScreenColData **terrain_map,
            int accumulated_x_offset, int accumulated_y_offset)
{
    ScreenColData *col_data;

    SDL_SetRenderDrawColor(pContext->renderer, 0, 0, 0, 255);
    SDL_RenderClear(pContext->renderer);
    for (int i = 0; i < ROW_COUNT; i++)
    {
        col_data = terrain_map[i];
        for (int j = 0; j < COL_COUNT; j++)
        {
            //printf("Col %d %d: %d %d", i, j, col_data->obj.rect.x, col_data->obj.rect.y);
            renderObj(pContext->renderer, &col_data->obj, accumulated_x_offset, accumulated_y_offset);
            col_data = col_data->next;
        }
    }
    renderObj(pContext->renderer, &pPlayer->obj, 0, 0); // Here the offsets are zero to prevent moving the player.
    SDL_RenderPresent(pContext->renderer);
}

static void handleKeyboard(int *pX_comp, int *pY_comp)
{
    const Uint8 *state = SDL_GetKeyboardState(NULL);

    if (state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP])
    {
        *pY_comp -= 1;
    }
    if (state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT])
    {
        *pX_comp -= 1;
    }
    if (state[SDL_SCANCODE_S] || state[SDL_SCANCODE_DOWN])
    {
        *pY_comp += 1;
    }
    if (state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT])
    {
        *pX_comp += 1;
    }
}

int handleEvents(int *pX_comp, int *pY_Comp)
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            return 0;
        }
    }
    handleKeyboard(pX_comp, pY_Comp);
    return 1;
}