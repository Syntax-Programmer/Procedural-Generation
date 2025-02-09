#pragma once

#include <stdint.h>
#include <SDL2/SDL.h>
#include "global.h"
#include "physics.h"

/// @brief The SDL context that is used to render upon the game.
typedef struct GraphicsContext {
    SDL_Window *win;
    SDL_Renderer *renderer;
} GraphicsContext;

/// @brief Initialize the GraphicsContext provided with newly created win and renderer.
/// @param pContext The context to initialize.
/// @param w The width of the window.
/// @param h The height of the window.
/// @param title The title of the window.
extern void initGraphicsContext(GraphicsContext *pContext, uint32_t w,
                                uint32_t h, const char *title);

/// @brief Renders the contents of the game.
/// @param renderer The renderer to render wrt.
/// @param terrain_map The terrain map to be rendered.
/// @param pWorld_pos_handle The pointer to the position handle to initialize with the map.
extern void render(SDL_Renderer *renderer, SDL_Texture **terrain_map, PosHandle *pWorld_pos_handle);

/// @brief Destroys the created GraphicsContext and cleans up SDL resources.
/// @param pMain_context The main initialized context of the game.
extern void exitGraphicsMgr(GraphicsContext *pMain_context);
