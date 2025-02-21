#include <SDL2/SDL_render.h>
#include <stdint.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "../include/global.h"
#include "../include/physics.h"
#include "../include/graphicsMgr.h"
#include "../include/mapMgr.h"

/*
 * @brief Checks for keyboard input and returns the concerned ones.
 *
 * Checks for keyboard input using **SDL_GetKeyboardState(NULL)**, then
 * checks for concerned inputs and returns them. Using 2 bytes int as it
 * the max required size for any input flag.
 *
 * @return The input bit flags defined in **global.h** ORed together.
 */
static uint16_t handleKeyboard();

/*
 * @brief Determines the input events performed by the player.
 *
 * Returns quitting event if received, otherwise checks for keyboard and
 * mouse inputs by their designated functions:
 * **handleKeyboard()** and **handleMouse()**.
 *
 * @return Bitmask of inputs done in a frame, or a quit flag if
 *         received.
 */
static uint16_t handleEvents();

/*
 * @brief Initializes all the resources, variables and libraries
 *        used throughout the game ensuring proper initialization.
 *
 * @param pSeed The pointer to the seed used to generated randomness in the code.
 * @param pContext The pointer to the **GraphicsContext** struct defined in
 *                 **graphicsMgr.h** that is used for rendering the game,
 * @param *pWorldPosHandler The pointer to the **PosHandle** struct defined in
 *                          **mapMgr,h** that is used to manage map and cam positions.
 * @param pTerrain_map The pointer the \\SDL_Texture **\\ that is the 1-D array that
 *                     will hold all the initial chunks of the map before any updates.
 *
 * @return A 0 for success, 1 if failure occurred. Logs will be printed as the errors occur,
 */
static uint8_t initGame(time_t *pSeed, GraphicsContext *pContext,
                        PosHandle *pWorld_pos_handle, SDL_Texture ***pTerrain_map);

/*
 * @brief Handles the updates for non-visible aspect of the code.
 *
 * Handles the variables, the game states and updates textures with new data etc.
 * Also handles the movement of the player, actions performed all through a single
 * interface hiding the complexity of the sub-steps needed.
 *
 * @param input_flags Bitmask of the inputs done by the user used to
 *                    determine what the player wants to do.
 * @param seed The seed for the current session of this code running used to update
 *             things that were generated with or require randomness.
 * @param *pWorldPosHandler The pointer to the **PosHandle** struct defined in
 *                          **mapMgr,h** that is used to manage map and cam positions.
 * @param terrain_map The 1-D array SDL_Texture pointer of the chunks of the
 *                    terrain being displayed.
 * @param delta_time The average that a frame is taking to execute.
 */
static void handleState(uint16_t input_flags, time_t seed, PosHandle *pWorld_pos_handle,
                        SDL_Texture **terrain_map, double delta_time);

/*
 * @brief Gameloop that executes the game logic.
 *
 * Performs the needed calculation and logic check, handles frame
 * rate and runs the game through a single interface that can be
 * called by the main function.
 *
 * @param seed The seed for the current session of this code running used to
 *             update/handle things that were generated with or require randomness.
 * @param pContext The pointer to the **GraphicsContext** struct defined in
 *                 **graphicsMgr.h** that is used for rendering the game,
 * @param *pWorldPosHandler The pointer to the **PosHandle** struct defined in
 *                          **mapMgr,h** that is used to manage map and cam positions.
 * @param terrain_map The 1-D array SDL_Texture pointer of the chunks of the
 *                    terrain being displayed.
 */
static void gameloop(time_t seed, GraphicsContext *pContext,
                     PosHandle *pWorld_pos_handle, SDL_Texture **terrain_map);

/*
 * @brief Cleans up the resources that explicitly require cleanup.
 *
 * Cleans up the mallocs done through out the code, and performs the
 * cleanup steps of the used library.
 *
 * @param pContext The pointer to the **GraphicsContext** struct defined in
 *                 **graphicsMgr.h** that is to be destroyed,
 * @param terrain_map The 1-D array SDL_Texture pointer of the chunks of the
 *                    terrain being displayed to be destroyed.
 */
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
    * I spent a entire fucking day trying to find out why the code
    * ```
    * SDL_Init(SDL_INIT_EVERYTHING);
    * ```
    * is causing mem leaks at 34 different locations.

    * Turns out I just have to recompile the code for the 20th time,
    * without changing even a single thing, and it just magically fixes it.

    * What did I debug, what I did wrong we will never know.
    * Just remember fellow dev, if it doesn't work just recompile it for 20 times
    * in a row and stop banging your head against the wall.
    */
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return 1;
    }
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "0");
    *pSeed = time(NULL);
    initGraphicsContext(pContext, SCREEN_WIDTH, SCREEN_HEIGHT, GAME_TITLE);
    if (pContext->renderer) { initTerrainMap(pContext->renderer, pTerrain_map, pWorld_pos_handle, *pSeed); }
    if (!pContext->win || !(*pTerrain_map)) { return 1; }

    return 0;
}

static void handleState(uint16_t input_flags, time_t seed, PosHandle *pWorld_pos_handle,
                        SDL_Texture **terrain_map, double delta_time) {
    IntVec2 delta_dist = {.x = 0, .y = 0};

    // Clearing conflicting movements also gives performance boost in the update terrain map function.
    if (HAS_FLAG(input_flags, LEFT) && HAS_FLAG(input_flags, RIGHT)) { CLEAR_FLAG(input_flags, LEFT | RIGHT); }
    if (HAS_FLAG(input_flags, UP) && HAS_FLAG(input_flags, DOWN)) { CLEAR_FLAG(input_flags, UP | DOWN); }
    //* A point for optimization as using SDL_Rect, out delta_dist is always 1 no matter the direction with 300 as vel below.
    if (input_flags) { delta_dist = computeMoveDist(300, delta_time, input_flags); }
    if (delta_dist.x || delta_dist.y) {
        pWorld_pos_handle->cam_pos.x += delta_dist.x;
        pWorld_pos_handle->cam_pos.y += delta_dist.y;
        updateTerrainMap(terrain_map, pWorld_pos_handle, input_flags, seed);
    }
}

static void gameloop(time_t seed, GraphicsContext *pContext,
                     PosHandle *pWorld_pos_handle, SDL_Texture **terrain_map) {
    uint16_t input_flags = 0;
    uint8_t frame_c = 0;
    double delta_time_sec = FRAME_MIN_TIME / 1000.0;
    Uint32 delta_time_ms_bffr = SDL_GetTicks(), frame_start = 0, frame_duration = 0;

    while (1) {
        frame_start = SDL_GetTicks();
        getDeltaTime(&delta_time_ms_bffr, &delta_time_sec, &frame_c);
        //if (!(frame_c % 10)) { printf("FPS: %f\n", 1.0/delta_time_sec); }
        input_flags = handleEvents();
        if (HAS_FLAG(input_flags, QUIT)) { return; }
        render(pContext->renderer, terrain_map, pWorld_pos_handle);
        handleState(input_flags, seed, pWorld_pos_handle, terrain_map, delta_time_sec);
        if ((frame_duration = (SDL_GetTicks() - frame_start)) < FRAME_MIN_TIME) { // FPS capping
            SDL_Delay(FRAME_MIN_TIME - frame_duration);
        }
        frame_c++;
    }
}

static void exitGame(GraphicsContext *pContext, SDL_Texture **terrain_map) {
    exitGraphicsMgr(pContext);
    freeTerrainMap(terrain_map);
}

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
