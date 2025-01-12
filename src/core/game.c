#include "core/game.h"

static int initGame(Context *pMain_context, int *pSeed, Player *pPlayer, ChunkData ***pTerrain_map);
static void gameLoop(int is_running, int seed, Context *pMain_context, Player *pPlayer,
                     ChunkData ***pTerrain_map);
static void exitGame(Context *pMain_context, ChunkData **terrain_map);

static int initGame(Context *pMain_context, int *pSeed, Player *pPlayer, ChunkData ***pTerrain_map) {
    int status = initGraphics();

    *pMain_context = createContext("A-RPG", SCREEN_WIDTH, SCREEN_HEIGHT);
    *pSeed = time(NULL);
    *pPlayer = createPlayer((SCREEN_WIDTH / 2) - (TILE_SQUARE_SIDE / 2),
                         (SCREEN_HEIGHT / 2) - (TILE_SQUARE_SIDE / 2), TILE_SQUARE_SIDE,
                         TILE_SQUARE_SIDE, 255, 255, 255, 200);
    *pTerrain_map = initTerrainTextureMap(pMain_context->renderer, *pSeed);
    if (!pMain_context->win || !*pTerrain_map) {
        status = 1;
    }
    return status;
}

static void gameLoop(int is_running, int seed, Context *pMain_context, Player *pPlayer,
                     ChunkData ***pTerrain_map) {
    int x_comp = 0, y_comp = 0, frame_c = 0;
    float cam_x = 0, cam_y = 0; // Total offset accumulated through moving;
    time_t start = time(NULL);
    double delta_time = 1.0 / 200.0;

    /*
    Using SDL_FRect creates the black lines due to floating point impresision.
    Using SDL_Rect and int vels will fix this issue. But we will lose movement and delta_Dist in <=0<=1 become zero.
    Maybe use SDL_Rect or the ceil() with int values.
    */

    while (is_running) {
        printf("FPS: %f\n", 1/delta_time);
        getDeltaTime(&start, &delta_time, &frame_c);
        if (handleEvents(&x_comp, &y_comp)) {
            break;
        }
        handleState(pPlayer, pTerrain_map, pMain_context->renderer, &x_comp, &y_comp, &cam_x,
                    &cam_y, delta_time, seed);
        render(pMain_context->renderer, pPlayer, *pTerrain_map, cam_x, cam_y);
        frame_c++;
    }
}

static void exitGame(Context *pMain_context, ChunkData **terrain_map) {
    quitGraphics(pMain_context);
    freeTerrainTextureMap(terrain_map);
    exit(0);
}

void game() {
    Context main_context;
    int seed;
    Player player;
    ChunkData **terrain_map;
    int is_running = !initGame(&main_context, &seed, &player, &terrain_map);

    gameLoop(is_running, seed, &main_context, &player, &terrain_map);
    exitGame(&main_context, terrain_map);
}