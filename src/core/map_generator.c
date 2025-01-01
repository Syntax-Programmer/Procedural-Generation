/*
    Used a techinque called Perlin Noise with Fractal Brownian Motion to generate the map.
    For reference here is the resource I used:
    https://rtouti.github.io/graphics/perlin-noise-algorithm
*/

#include "map_generator.h"

static Vector2D getConstVector(int v);
static void getPTableValues(int* pVtr, int* pVtl, int* pVbr, int* pVbl, const int* pP_table, int X, int Y);
static float fade(float t);
static float lerp(float t, float a1, float a2);
static void shufflePTable(int* pP_table);
static int* initPTable();
static float perlinNoise(float x, float y, const int *pP_table);
static float FractalBrownainNoise(int x, int y, int octaves, const int* pP_table);
static int generate_pixel_colors(Uint32 pixel_colors[MAP_HEIGHT][MAP_WIDTH]);
static int allocObj(Obj** terrain_map, int* pTerrain_map_i, int x, int y, int w,
                    int h, Uint32 color_hex, int can_collide);

static Vector2D getConstVector(int v) {
    int x = v % 3;

    switch (x) {
        case 0:
            return createVector2D(1, 1);
            break;
        case 1:
            return createVector2D(1, -1);
            break;
        case 2:
            return createVector2D(-1, 1);
            break;
        default:
            return createVector2D(-1, -1);
            break;
    }
}

static void getPTableValues(int* pVtr, int* pVtl, int* pVbr, int* pVbl, const int* pP_table, int X, int Y) {
    *pVtr = pP_table[pP_table[X + 1] + Y + 1];
    *pVtl = pP_table[pP_table[X] + Y + 1];
    *pVbr = pP_table[pP_table[X + 1] + Y];
    *pVbl = pP_table[pP_table[X] + Y];
}

static float fade(float t) {
    return ((6*t - 15)*t + 10)*t*t*t;
}

static float lerp(float t, float a1, float a2) {
    return a1 + t * (a2 - a1);
}

static void shufflePTable(int* pP_table) {
    int index, temp;

    for (int i = PERLIN_NOISE_WRAP_LIMIT; i > 0; i--) {
        index = rand() % i;
        temp = pP_table[i];
        pP_table[i] = pP_table[index];
        pP_table[index] = temp;
    }
}

static int* initPTable() {
    int* pP_table = malloc((PERLIN_NOISE_WRAP_LIMIT + 1) * 2 * sizeof(int));

    if (!pP_table) {
        fprintf(stderr, "Failed to allocate memory for permutation table\n");
        return NULL;
    }
    for (int i = 0; i <= PERLIN_NOISE_WRAP_LIMIT; i++) {
        pP_table[i] = i;
    }
    shufflePTable(pP_table);
    for (int i = 0; i <= PERLIN_NOISE_WRAP_LIMIT; i++) {
        pP_table[i + PERLIN_NOISE_WRAP_LIMIT + 1] = pP_table[i];
    }
    return pP_table;
}


static float perlinNoise(float x, float y, const int *pP_table) {
    int X = (int)floor(x) & (PERLIN_NOISE_WRAP_LIMIT + 1), Y = (int)floor(y) & (PERLIN_NOISE_WRAP_LIMIT + 1);
    float xf = x - floor(x), yf = y - floor(y);
    // To perform non-lineal interpolation.
    float u = fade(xf), v = fade(yf);

    Vector2D tr = createVector2D(xf - 1, yf - 1), tl = createVector2D(xf, yf - 1),
             br = createVector2D(xf - 1, yf), bl = createVector2D(xf, yf); // top right, top left, bottom right, bottom left

    int vtr, vtl, vbr, vbl;
    getPTableValues(&vtr, &vtl, &vbr, &vbl, pP_table, X, Y);
    Vector2D ctr = getConstVector(vtr), ctl = getConstVector(vtl),
             cbr = getConstVector(vbr), cbl = getConstVector(vbl); // constant tr, constant tl, constant br, constant bl

    float dtr = vectorDotProduct(&tr, &ctr), dtl = vectorDotProduct(&tl, &ctl),
        dbr = vectorDotProduct(&br, &cbr), dbl = vectorDotProduct(&bl, &cbl); // dot tr, dot tl, dot br, dot bl;
    return lerp(u,
                lerp(v, dtr, dtl),
                lerp(v, dbr, dbl)
            );
}

static float FractalBrownainNoise(int x, int y, int octaves, const int* pP_table) {
    float result = 0.0f, amplitude = 1.0f, frequency = 0.005f, n = 0.0f;


    for (int oct = 0; oct < octaves; oct++) {
        n = amplitude * perlinNoise(x * frequency, y * frequency, pP_table);
        result += n;
        amplitude *= 0.5f;
        frequency *= 2.0f;
    }
    return result;
}

static int generate_pixel_colors(Uint32 pixel_colors[MAP_HEIGHT][MAP_WIDTH]) {
    float n;
    int c;
    int* pP_table = initPTable();
    if (!pP_table) {
        return 0;
    }

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            n = FractalBrownainNoise(x, y, FBM_OCTAVES, pP_table);
            // Restircting the value of n to be between 0 and 1 instead of -1 and 1
            n += 1.0;
            n /= 2.0;
            c = (int)(n * 255);
            pixel_colors[y][x] = encodeColor(c, c, c, 255);
        }
    }
    free(pP_table);
    return 1;
}

void freeTerrainMap(Obj** terrain_map) {
    if (terrain_map) {
        for (int i = 0; terrain_map[i] != NULL; i++) {
            free(terrain_map[i]);
        }
        free(terrain_map);
    }
}


static int allocObj(Obj** terrain_map, int* pTerrain_map_i, int x, int y, int w,
                    int h, Uint32 color_hex, int can_collide) {
    Obj* obj = malloc(sizeof(Obj));

    if (!obj) {
        fprintf(stderr, "Can't malloc space for a object int the lvl data.\n");
        terrain_map[*pTerrain_map_i] = NULL;
        freeTerrainMap(terrain_map);
        return 0;
    }
    *obj = createObj(x, y, w, h, color_hex, can_collide);
    terrain_map[(*pTerrain_map_i)++] = obj;
    return 1;
}

Obj** generateTerrainMap() {
    int map_i = 0;
    Obj** terrain_map = malloc((MAP_HEIGHT* MAP_WIDTH + 1) * sizeof(Obj*)); // +1 for NULL sentinal
    Uint32 pixel_colors[MAP_HEIGHT][MAP_WIDTH];

    if (!terrain_map) {
        fprintf(stderr, "Failed to allocate memory for terrain map\n");
        return NULL;
    }
    if (!generate_pixel_colors(pixel_colors)) {
        freeTerrainMap(terrain_map);
        return NULL;
    }
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (!allocObj(terrain_map, &map_i, x, y, 1, 1, pixel_colors[y][x], 0)) {
                return NULL;
            }
        }
    }
    terrain_map[map_i] = NULL;
    return terrain_map;
}