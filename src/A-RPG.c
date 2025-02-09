#include <SDL2/SDL_render.h>
#include <stdint.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "../include/global.h"
#include "../include/physics.h"
#include "../include/graphicsMgr.h"
#include "../include/mapMgr.h"

/// @brief Gives us the valid input that are currently being done on the keyboard.
/// @return A 2 byte of bitwise flags representing key presses.
static uint16_t handleKeyboard();

/// @brief Handles the events done by the player.
/// @return A 2 byte of bitwise flags representing key presses.
static uint16_t handleEvents();

/// @brief Initializes all the resources of the game and the required libraries.
/// @param pSeed The random seed that will be used by our noise functions.
/// @param pContext The pointer to the GraphicsContext struct to be initialized.
/// @param pWorld_pos_handle The pointer to the position handle to initialize with the map.
/// @param pTerrain_map The pointer to the terrain_map to be initialized.
/// @return 0 for success, 1 for failure.
static uint8_t initGame(time_t *pSeed, GraphicsContext *pContext,
                        PosHandle *pWorld_pos_handle, SDL_Texture ***pTerrain_map);

/// @brief Handles the game state variable for if they need updating.
/// @param input_flags The input flags are bit flags that tell how the particle is moving.
/// @param seed The random seed set at the program starting.
/// @param pWorld_pos_handle The pointer to the position handle to initialize with the map.
/// @param terrain_map The fully noised upon and initialized terrain map.
/// @param delta_time The average time a frame takes to execute.
static void handleState(uint16_t input_flags, time_t seed, PosHandle *pWorld_pos_handle,
                        SDL_Texture **terrain_map, double delta_time);

/// @brief The main gameloop that uses the resources initialized before.
/// @param seed The random seed we initialized.
/// @param pContext The initialized GraphicsContext.
/// @param pWorld_pos_handle The pointer to the position handle to initialize with the map.
/// @param terrain_map The fully noised upon and initialized terrain map.
static void gameloop(time_t seed, GraphicsContext *pContext,
                     PosHandle *pWorld_pos_handle, SDL_Texture **terrain_map);

/// @brief Cleans up the resources that were initialized by initGame.
/// @param pContext The context that was used to render stuff.
/// @param terrain_map The terrain map that was created.
static void exitGame(GraphicsContext *pContext, SDL_Texture **terrain_map);

static uint16_t handleKeyboard() {
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    uint16_t flags = 0;

    if (state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP]) { SET_FLAG(flags, UP); }
    if (state[SDL_SCANCODE_S] || state[SDL_SCANCODE_DOWN]) { SET_FLAG(flags, DOWN); }
    if (state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT]) { SET_FLAG(flags, LEFT); }
    if (state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT]) { SET_FLAG(flags, RIGHT); }
    if (state[SDL_SCANCODE_ESCAPE]) { SET_FLAG(flags, ESC); }
    if (state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL]) { SET_FLAG(flags, CTRL); }
    if (state[SDL_SCANCODE_LSHIFT] || state[SDL_SCANCODE_RSHIFT]) { SET_FLAG(flags, SHIFT); }
    if (state[SDL_SCANCODE_SPACE]) { SET_FLAG(flags, SPACE); }

    return flags;
}

static uint16_t handleEvents() {
    SDL_Event event;

    if (SDL_PollEvent(&event) && event.type == SDL_QUIT) { return QUIT; }
    return handleKeyboard(); // TODO: Make it handleKeyboard() | handleMouse();
}

static uint8_t initGame(time_t *pSeed, GraphicsContext *pContext,
                        PosHandle *pWorld_pos_handle, SDL_Texture ***pTerrain_map) {
    /*
    I spent a entire fucking day trying to find out why the code
    ```
    SDL_Init(SDL_INIT_EVERYTHING);
    ```
    is causing mem leaks at 34 different locations.

    Turns out I just have to recompile the code for the 20th time,
    without changing even a single thing, and it just magically fixes it.

    What did I debug, what I did wrong we will never know.
    Just remember fellow dev, if it doesn't work just recompile it for 20 times
    in a row and stop banging your head against the wall.
    */
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return 1;
    }
    *pSeed = time(NULL);
    initGraphicsContext(pContext, SCREEN_WIDTH, SCREEN_HEIGHT, GAME_TITLE);
    if (pContext->renderer) { initTerrainMap(pContext->renderer, pTerrain_map, pWorld_pos_handle, *pSeed); }
    if (!pContext->win || !(*pTerrain_map)) { return 1; }

    return 0;
}

static void handleState(uint16_t input_flags, time_t seed, PosHandle *pWorld_pos_handle,
                        SDL_Texture **terrain_map, double delta_time) {
    Vec2 delta_dist = {.x = 0, .y = 0};

    if (input_flags) { delta_dist = computeMoveDist(300, delta_time, input_flags); }
    if (delta_dist.x || delta_dist.y) {
        pWorld_pos_handle->cam_x += delta_dist.x;
        pWorld_pos_handle->cam_y += delta_dist.y;
    }
}

static void gameloop(time_t seed, GraphicsContext *pContext,
                     PosHandle *pWorld_pos_handle, SDL_Texture **terrain_map) {
    uint16_t input_flags = 0;
    uint8_t frame_c = 0;
    double delta_time = FRAME_MIN_TIME / 1000.0;
    Uint32 delta_time_ms_bffr = SDL_GetTicks(), frame_start = 0, frame_duration = 0;

    while (1) {
        frame_start = SDL_GetTicks();
        getDeltaTime(&delta_time_ms_bffr, &delta_time, &frame_c);
        if (!(frame_c % 10)) { printf("FPS: %f\n", 1.0/delta_time); }
        input_flags = handleEvents();
        if (HAS_FLAG(input_flags, QUIT)) { return; }
        render(pContext->renderer, terrain_map, pWorld_pos_handle);
        handleState(input_flags, seed, pWorld_pos_handle, terrain_map, delta_time);
        if ((frame_duration = SDL_GetTicks() - frame_start) < FRAME_MIN_TIME) { // FPS capping
            SDL_Delay(FRAME_MIN_TIME - frame_duration);
        }
        frame_c++;
    }
}

static void exitGame(GraphicsContext *pContext, SDL_Texture **terrain_map) {
    exitGraphicsMgr(pContext);
    freeTerrainMap(terrain_map);
}

/// @brief The game that initializes, plays and cleans up the game.
/// @return C standard.
int main() {
    time_t seed;
    GraphicsContext main_context;
    SDL_Texture **terrain_map = NULL;
    PosHandle world_pos_handle;
    uint8_t failure = initGame(&seed, &main_context, &world_pos_handle, &terrain_map);

    if (!failure) {
        gameloop(seed, &main_context, &world_pos_handle ,terrain_map);
    }
    exitGame(&main_context, terrain_map);

    return 0;
}
