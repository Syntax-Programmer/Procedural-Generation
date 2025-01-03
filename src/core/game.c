#include "game.h"

static int initGame(GameContext *pMain_context, Player *pPlayer, ScreenColData ***pTerrainMap, int **ptr_pP_table);
static void gameloop(int is_running, GameContext *pMain_context, Player *pPlayer, ScreenColData **terrain_map);
static void exitGame(GameContext *pMain_context, ScreenColData **terrain_map, int *pP_table);

static int initGame(GameContext *pMain_context, Player *pPlayer, ScreenColData ***pTerrainMap, int **ptr_pP_table)
{
    int status;

    status = initSDL();
    *pMain_context = createGameContext("MyGame", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                       SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
    *pPlayer = createPlayer((SCREEN_WIDTH / 2) - (TILE_SQUARE_SIDE / 2), (SCREEN_HEIGHT / 2) - (TILE_SQUARE_SIDE / 2), // Ensures that the player is centered.
                            TILE_SQUARE_SIDE, TILE_SQUARE_SIDE, 0xddcb33ff, 1, 100, 200);
    *ptr_pP_table = initPTable();
    *pTerrainMap = initScreenData(*ptr_pP_table, PERLIN_TERRAIN_FREQ);
    if (!pMain_context->win || !(*pTerrainMap) || !(*ptr_pP_table))
    {
        status = 0;
    }
    return status;
}

static void gameloop(int is_running, GameContext *pMain_context, Player *pPlayer, ScreenColData **terrain_map)
{
    time_t start = time(NULL);
    double delta_time = 1.0 / 60.0;
    int frame_c = 0, mov_x_comp = 0, mov_y_comp = 0, accumulated_x_offset = 0,
        accumulated_y_offset = 0; // Total offset accumulated through moving

    while (is_running)
    {
        getDeltaTime(&start, &delta_time, &frame_c);
        if (!handleEvents(&mov_x_comp, &mov_y_comp))
        {
            is_running = 0;
            break;
        }
        /*
        TODO: Set a FPS cap of around 60-120 fps and then REMOVE THE VSYNC FROM THE renderer constraints while creating.
        TODO: Make a game_state_handler moduele.
        */
        handleState(pPlayer, &mov_x_comp, &mov_y_comp, &accumulated_x_offset,
                    &accumulated_y_offset, delta_time);
        render(pMain_context, pPlayer, terrain_map, accumulated_x_offset, accumulated_y_offset);
        frame_c++;
    }
}

static void exitGame(GameContext *pMain_context, ScreenColData **terrain_map, int *pP_table)
{
    destroyGameContext(pMain_context);
    SDL_Quit();
    free(pP_table);
    freeScreenData(terrain_map);
}

void game()
{
    GameContext main_context;
    Player player;
    int *pP_table;
    ScreenColData **terrain_map;
    int is_running = initGame(&main_context, &player, &terrain_map, &pP_table);

    gameloop(is_running, &main_context, &player, terrain_map);
    exitGame(&main_context, terrain_map, pP_table);
}