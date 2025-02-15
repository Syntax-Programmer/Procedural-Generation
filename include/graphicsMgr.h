#pragma once

#include <stdint.h>
#include <SDL2/SDL.h>
#include "global.h"
#include "physics.h"

/*
 * @brief Holds the window and renderer of the graphics context.
 *
 * Initialized during runtime and stays alive till the game exits.
 *
 * @elem win The SDL_Window.
 * @elem renderer The SDL_Renderer used to render things. (Wow didn't expect a renderer to render.)
 */
typedef struct GraphicsContext {
    SDL_Window *win;
    SDL_Renderer *renderer;
} GraphicsContext;

/*
 * @brief Initializes the GraphicsContext provided.
 *
 * Initializes by creating a new SDL_Window and SDL_Renderer.
 * The win and renderer are NULL if any error occurs.
 *
 * @param pContext The GraphicsContext that is to be initialized.
 * @param w The width to set for the game window.
 * @param h The height to set for the game window.
 * @param title The title to set for the game window.
 */
extern void initGraphicsContext(GraphicsContext *pContext, uint32_t w,
                                uint32_t h, const char *title);

/*
 * @brief Renders the contents of the game.
 *
 * Renders the player, the terrain, NPCs, decoration etc each frame,
 * providing a single function call used to render everything needed.
 *
 * @param renderer The SDL_Renderer from the created GraphicsContext
 *                 that the rendering will take place upon.
 * @param terrain_map The 1-D array SDL_Texture pointer of the chunks of the
 *                    terrain to be rendered.
 * @param *pWorldPosHandler The pointer to the **PosHandle** struct defined in
 *                          **mapMgr,h** that is used to manage map and cam positions.
 */
extern void render(SDL_Renderer *renderer, SDL_Texture **terrain_map, PosHandle *pWorld_pos_handle);

/*
 * @brief Exits the graphicsMgr and cleans up resources used.
 *
 * Destroys the GraphicsContext created and invoking the quit
 * functions of SDL.
 *
 * @param pMain_context The main context created during initialization
 *                      phase of the game.
 */
extern void exitGraphicsMgr(GraphicsContext *pMain_context);
