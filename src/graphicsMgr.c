#include "../include/graphicsMgr.h"

/*
 * @brief Attemps to destroy/free any contents of the GraphicsContext that
 *        are initialized during this function's call.
 *
 * @param pTo_destroy The GraphicsContext whose contents to be destroyed
 */
static void destroyContext(GraphicsContext *pTo_destroy);

static void destroyContext(GraphicsContext *pTo_destroy) {
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

void initGraphicsContext(GraphicsContext *pContext, uint32_t w,
                         uint32_t h, const char *title) {
    pContext->win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                   w, h, SDL_WINDOW_SHOWN);
    if (!pContext->win) {
        SDL_Log("Unable to create SDL_Window: %s", SDL_GetError());
        return;
    }
    pContext->renderer = SDL_CreateRenderer(pContext->win, -1, SDL_RENDERER_ACCELERATED);
    if (!pContext->renderer) {
        SDL_Log("Unable to create SDL_Renderer: %s", SDL_GetError());
        destroyContext(pContext);
    }
}

void render(SDL_Renderer *renderer, SDL_Texture **terrain_map, PosHandle *pWorld_pos_handle) {
    int32_t initial_x_pos = -CHUNK_SIZE - pWorld_pos_handle->cam_pos.x,
            j = pWorld_pos_handle->tl_chunk_index, row_progress = 0; // To improve performance by reducing redundant subtraction.
    SDL_Rect terrain_chunk_pos = {.w = CHUNK_SIZE, .h = CHUNK_SIZE,
                                  .x = initial_x_pos,
                                  .y = -CHUNK_SIZE - pWorld_pos_handle->cam_pos.y};

    SDL_RenderClear(renderer);
    for (int32_t i = 0; i < CHUNK_C; i++, j++) {
        /* Using j % CHUNK_C to instead of shifting the elemets in the array,
        We logically move the starting index and let the index overflow/underflow to wrap around the array.*/
        SDL_RenderCopy(renderer, terrain_map[j % CHUNK_C], NULL, &terrain_chunk_pos);
        terrain_chunk_pos.x += CHUNK_SIZE;
        if (++row_progress == COL_C) { // Avoiding modulo for performance.
            terrain_chunk_pos.y += CHUNK_SIZE;
            terrain_chunk_pos.x = initial_x_pos;
            row_progress = 0;
        }
    }
    SDL_RenderPresent(renderer);
}

void exitGraphicsMgr(GraphicsContext *pMain_context) {
    destroyContext(pMain_context);
    if (SDL_WasInit(0)) {
        SDL_QuitSubSystem(SDL_INIT_EVERYTHING);
    }
}
