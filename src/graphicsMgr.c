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
    pContext->renderer = SDL_CreateRenderer(pContext->win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    if (!pContext->renderer) {
        SDL_Log("Unable to create SDL_Renderer: %s", SDL_GetError());
        destroyContext(pContext);
    }
}

void render(SDL_Renderer *renderer, SDL_Texture **terrain_map, PosHandle *pWorld_pos_handle) {
    SDL_Rect terrain_chunk_pos = {.w = CHUNK_SIZE, .h = CHUNK_SIZE,
                                    //! Change it to -CHUNK_SIZE and -CHUNK_SIZE when updating the terrain map feature is implemented.
                                  .x = pWorld_pos_handle->origin_chunk_pos.x - pWorld_pos_handle->cam_pos.x,
                                  .y = pWorld_pos_handle->origin_chunk_pos.y - pWorld_pos_handle->cam_pos.y};

    SDL_RenderClear(renderer);
    for (int i = 0; i < CHUNK_C; i++) {
        if (i && !(i % COL_C)) { // Not 0 and next row.
            terrain_chunk_pos.y += CHUNK_SIZE;
            //! Here also to -CHUNK_SIZE.
            terrain_chunk_pos.x = pWorld_pos_handle->origin_chunk_pos.x - pWorld_pos_handle->cam_pos.x;
        }
        SDL_RenderCopy(renderer, terrain_map[i], NULL, &terrain_chunk_pos);
        terrain_chunk_pos.x += CHUNK_SIZE;
    }
    SDL_RenderPresent(renderer);
}

void exitGraphicsMgr(GraphicsContext *pMain_context) {
    destroyContext(pMain_context);
    if (SDL_WasInit(0)) {
        SDL_QuitSubSystem(SDL_INIT_EVERYTHING);
    }
}
