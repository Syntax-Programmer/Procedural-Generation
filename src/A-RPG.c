#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <time.h>
#include "global.h"
#include "physics.h"
#include "graphics.h"
#include "map.h"
#include "obj.h"
#include "stateManager.h"

static uint16_t handleKeyboard();
static uint16_t handleEvents();
static uint8_t initGame(time_t *pSeed, GraphicsContext *pContext, Player *pPlayer,
                        SDL_Texture ****pTerrain_map, SDL_Rect *pOrigin_rect);
static void gameloop(time_t seed, GraphicsContext *pContext, Player *pPlayer,
                     SDL_Texture ***terrain_map, SDL_Rect *pOrigin_rect);
static void exitGame(GraphicsContext *pContext, SDL_Texture ***terrain_map);


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

static uint8_t initGame(time_t *pSeed, GraphicsContext *pContext, Player *pPlayer,
                        SDL_Texture ****pTerrain_map, SDL_Rect *pOrigin_rect) {
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        fprintf(stderr, "Unable to initialize SDL.\n");
        return 1;
    }
    *pSeed = time(NULL);
    *pContext = createContext(SCREEN_WIDTH, SCREEN_HEIGHT, GAME_TITLE);
    *pPlayer = createPlayer(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 32, 32,
                            106, 129, 158, 300, 128);
    *pTerrain_map = initTerrainMap(pContext->renderer, pOrigin_rect, *pSeed);
    if (!pContext->win || !*pTerrain_map) { return 1; }

    return 0;
}

static void gameloop(time_t seed, GraphicsContext *pContext, Player *pPlayer,
                     SDL_Texture ***terrain_map, SDL_Rect *pOrigin_rect) {
    uint16_t input_flags = 0;
    uint8_t frame_c = 0;
    double delta_time = FRAME_MIN_TIME / 1000.0;
    Uint32 delta_time_ms_bffr = SDL_GetTicks(), frame_start = 0, frame_duration = 0;

    while (1) {
        frame_start = SDL_GetTicks();
        getDeltaTime(&delta_time_ms_bffr, &delta_time, &frame_c);
        if (!frame_c % 10) { printf("FPS: %f\n", 1.0/delta_time); }
        input_flags = handleEvents();
        if (HAS_FLAG(input_flags, QUIT)) { return; }
        handleState(input_flags, seed, pOrigin_rect, terrain_map, pPlayer, delta_time);
        render(pContext->renderer, terrain_map, pOrigin_rect, pPlayer);
        if ((frame_duration = SDL_GetTicks() - frame_start) < FRAME_MIN_TIME) { // FPS capping
            SDL_Delay(FRAME_MIN_TIME - frame_duration);
        }
        frame_c++;
    }
}

static void exitGame(GraphicsContext *pContext, SDL_Texture ***terrain_map) {
    exitGraphicsMgr(pContext);
    freeTerrainMap(terrain_map);
}

int main() {
    time_t seed;
    GraphicsContext main_context;
    SDL_Texture ***terrain_map = NULL;
    SDL_Rect origin_rect;
    Player player;
    uint8_t running = !initGame(&seed, &main_context, &player, &terrain_map, &origin_rect);

    if (running) {
        gameloop(seed, &main_context, &player, terrain_map, &origin_rect);
    }
    exitGame(&main_context, terrain_map);

    return 0;
}