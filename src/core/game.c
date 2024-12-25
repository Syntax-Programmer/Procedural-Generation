#include "game.h"

static int initGame(GameContext* pMain_context, Player* pPlayer, Obj*** pLvl_data);
static void gameloop(int is_running, GameContext* pMain_context, Player* pPlayer, Obj** lvl_data);
static void exitGame(GameContext* pMain_context, Obj** lvl_data);

static int initGame(GameContext* pMain_context, Player* pPlayer, Obj*** pLvl_data) {
    int status;

    status = initSDL();
    *pMain_context = createGameContext("MyGame", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
    *pPlayer = createPlayer((SCREEN_WIDTH / 2) - (PLAYER_WIDTH / 2), (SCREEN_HEIGHT / 2) - (PLAYER_HEIGHT / 2), // Ensures that the player is centered.
                            PLAYER_WIDTH, PLAYER_HEIGHT, 255, 0, 0, 255, 100, 200);
    *pLvl_data = parseLevel(0);
    if (!pMain_context->win || !(*pLvl_data)) {
        status = 0;
    }
    return status;
}

static void gameloop(int is_running, GameContext* pMain_context, Player* pPlayer, Obj** lvl_data) {
    time_t start = time(NULL);
    double delta_time = 1.0 / 60.0;
    int frame_c = 0, mov_x_comp = 0, mov_y_comp = 0, x_offset = 0, y_offset = 0;

    while (is_running) {
        getDeltaTime(&start, &delta_time, &frame_c);
        printf("FPS: %f\n", delta_time);
        if (!handleEvents(&mov_x_comp, &mov_y_comp)) {
            is_running = 0;
            break;
        }
        /*
        TODO: Set a FPS cap of around 60-120 fps and then REMOVE THE VSYNC FROM THE renderer constraints while creating.
        TODO: Make a game_state_handler moduele.
        */
        getMovOffset(pPlayer->vel, delta_time, mov_x_comp, mov_y_comp, &x_offset, &y_offset);
        render(pMain_context, pPlayer, lvl_data, x_offset, y_offset);
        x_offset = y_offset = 0;
        frame_c++;
    }
}

static void exitGame(GameContext* pMain_context, Obj** lvl_data) {
    destroyGameContext(pMain_context);
    SDL_Quit();
    freeLevelData(lvl_data);
}

void game() {
    GameContext main_context;
    Player player;
    Obj** lvl_data;
    int is_running = initGame(&main_context, &player, &lvl_data);

    gameloop(is_running, &main_context, &player, lvl_data);
    exitGame(&main_context, lvl_data);
}