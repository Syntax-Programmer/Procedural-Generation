#include "core/graphics_handler.h"

static void renderObj(SDL_Renderer *renderer, Obj *pTo_move);
static void handleKeyboard(int *pX_comp, int *pY_comp);

void destroyContext(Context* pTo_destroy) {
    if (pTo_destroy->win) {
        SDL_DestroyWindow(pTo_destroy->win);
        pTo_destroy->win = NULL;
    }
    if (pTo_destroy->renderer) {
        SDL_DestroyRenderer(pTo_destroy->renderer);
        pTo_destroy->renderer = NULL;
    }
}

Context createContext(const char *title, int w, int h) {
    Context context = {.win = NULL, .renderer = NULL};

    context.win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_ALLOW_HIGHDPI);
    if (!context.win) {
        fprintf(stderr, "ERROR: Failed in creating window for context with title: %s\n", title);
        return context;
    }
    context.renderer = SDL_CreateRenderer(context.win, -1, SDL_RENDERER_ACCELERATED);
    if (!context.renderer) {
        fprintf(stderr, "ERROR: Failed in creating renderer for context with title: %s\n", title);
        destroyContext(&context);
        return context;
    }
    return context;
}

int initGraphics() {
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        fprintf(stderr, "ERROR: Can't initialize graphics subsystem.\n");
        return 1;
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    return 0;
}

void quitGraphics(Context *pMain_context) {
    destroyContext(pMain_context);
    SDL_Quit();
}

static void renderObj(SDL_Renderer *renderer, Obj *pTo_move) {
    SDL_SetRenderDrawColor(renderer, pTo_move->r, pTo_move->g, pTo_move->b, 255);
    SDL_RenderFillRectF(renderer, &pTo_move->rect);
}

void render(SDL_Renderer *renderer, Player *pPlayer, ChunkData **terrain_map,
            float cam_x, float cam_y) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    for (int y = 0; y < ROW_CHUNK_COUNT; y++) {
        for (int x = 0; x < COL_CHUNK_COUNT; x++) {
            setRectPos(&terrain_map[y][x].dest, terrain_map[y][x].dest.x - cam_x, terrain_map[y][x].dest.y - cam_y);
            SDL_RenderCopyF(renderer, terrain_map[y][x].texture, NULL, &(terrain_map[y][x].dest));
            setRectPos(&terrain_map[y][x].dest, terrain_map[y][x].dest.x + cam_x, terrain_map[y][x].dest.y + cam_y);
        }
    }
    renderObj(renderer, &pPlayer->obj);
    SDL_RenderPresent(renderer);
}

static void handleKeyboard(int *pX_comp, int *pY_comp) {
    const Uint8 *state = SDL_GetKeyboardState(NULL);

    if (state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP]) {
        *pY_comp -= 1;
    }
    if (state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT]) {
        *pX_comp -= 1;
    }
    if (state[SDL_SCANCODE_S] || state[SDL_SCANCODE_DOWN]) {
        *pY_comp += 1;
    }
    if (state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT]) {
        *pX_comp += 1;
    }
}

int handleEvents(int *pX_comp, int *pY_Comp) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return 1;
        }
    }
    handleKeyboard(pX_comp, pY_Comp);
    return 0;
}