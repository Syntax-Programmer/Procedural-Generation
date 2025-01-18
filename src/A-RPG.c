#include <time.h>
#include "utils/globals.h"
#include "utils/phy.h"
#include "utils/mapGen.h"
#include "utils/obj.h"
#include "core/graphicsMgr.h"
#include "core/stateMgr.h"

static uint16_t handleKeyboard();
static uint16_t handleEvents();
static int initGame(GraphicsContext *pContext, int *pSeed, Chunk ***pTerrain_map,
                    Player *pPlayer);
static void gameLoop(GraphicsContext *pContext, int seed, Chunk **terrain_map,
                     Player *pPlayer);
static void exitGame(GraphicsContext *pContext, Chunk **terrain_map);

static uint16_t handleKeyboard() {
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    uint16_t flags = 0;

    if (state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP]) { SET_FLAG(flags, KEY_UP); }
    if (state[SDL_SCANCODE_S] || state[SDL_SCANCODE_DOWN]) { SET_FLAG(flags, KEY_DOWN); }
    if (state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT]) { SET_FLAG(flags, KEY_LEFT); }
    if (state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT]) { SET_FLAG(flags, KEY_RIGHT); }
    if (state[SDL_SCANCODE_ESCAPE]) { SET_FLAG(flags, KEY_ESC); }
    if (state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL]) { SET_FLAG(flags, KEY_CTRL); }
    if (state[SDL_SCANCODE_LSHIFT] || state[SDL_SCANCODE_RSHIFT]) { SET_FLAG(flags, KEY_SHIFT); }
    if (state[SDL_SCANCODE_SPACE]) { SET_FLAG(flags, KEY_SPACE); }

    return flags;
}

static uint16_t handleEvents() {
    SDL_Event event;

    if (SDL_PollEvent(&event) && event.type == SDL_QUIT) { return KEY_QUIT; }
    return handleKeyboard(); // TODO: Make it handleKeyboard() | handleMouse();
}

static int initGame(GraphicsContext *pContext, int *pSeed, Chunk ***pTerrain_map,
                    Player *pPlayer) {
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        fprintf(stderr, "Unable to initialize SDL.\n");
        return 1;
    }
    *pContext = createContext(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_TITLE);
    *pSeed = time(NULL);
    *pTerrain_map = initTerrainMap(pContext->renderer, *pSeed);
    *pPlayer = createPlayer(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 32, 32,
                            255, 0, 255, 300, 128);
    if (!pContext->win || !*pTerrain_map) { return 1; }

    return 0;
}

static void gameLoop(GraphicsContext *pContext, int seed, Chunk **terrain_map,
                     Player *pPlayer) {
    int frame_c = 0, cam_x = 0, cam_y = 0;
    uint16_t input_flags = 0;
    double delta_time = 1.0 / 200;
    Uint32 delta_time_ms_bffr = SDL_GetTicks(), frame_start = 0, frame_duration = 0;

    while (1) {
        frame_start = SDL_GetTicks();
        getDeltaTime(&delta_time_ms_bffr, &delta_time, &frame_c);
        printf("%f\n", 1.0/delta_time);

        input_flags = handleEvents();
        if (HAS_FLAG(input_flags, KEY_QUIT)) { break; }
        handleState(pContext->renderer, terrain_map, pPlayer, delta_time,
                    input_flags, seed, &cam_x, &cam_y);
        render(pContext->renderer, terrain_map, pPlayer, cam_x, cam_y);

        if ((frame_duration = SDL_GetTicks() - frame_start) < FRAME_MIN_TIME) {
            SDL_Delay(FRAME_MIN_TIME - frame_duration);
        }
        frame_c++;
    }
}

static void exitGame(GraphicsContext *pContext, Chunk **terrain_map) {
    exitGraphicsMgr(pContext);
    freeChunkMap(terrain_map);
}

int main() {
    GraphicsContext main_context;
    int seed = 0;
    Chunk **terrain_map = NULL;
    Player player;
    int failure = initGame(&main_context, &seed, &terrain_map, &player);

    if (!failure) { gameLoop(&main_context, seed, terrain_map, &player); }
    exitGame(&main_context, terrain_map);

    return 0;
}