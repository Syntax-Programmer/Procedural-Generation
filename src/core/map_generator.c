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
static float perlinNoise(float x, float y, float freq, int depth);
static float FractalBrownainNoise(int x, int y, int octaves, const int* pP_table);
static int generate_pixel_colors(Uint32 pixel_colors[MAP_HEIGHT][MAP_WIDTH]);
static int allocObj(Obj** terrain_map, int* pTerrain_map_i, int x, int y, int w,
                    int h, Uint32 color_hex, int can_collide);


static int hash[] = {208,34,231,213,32,248,233,56,161,78,24,140,71,48,140,254,245,255,247,247,40,
                     185,248,251,245,28,124,204,204,76,36,1,107,28,234,163,202,224,245,128,167,204,
                     9,92,217,54,239,174,173,102,193,189,190,121,100,108,167,44,43,77,180,204,8,81,
                     70,223,11,38,24,254,210,210,177,32,81,195,243,125,8,169,112,32,97,53,195,13,
                     203,9,47,104,125,117,114,124,165,203,181,235,193,206,70,180,174,0,167,181,41,
                     164,30,116,127,198,245,146,87,224,149,206,57,4,192,210,65,210,129,240,178,105,
                     228,108,245,148,140,40,35,195,38,58,65,207,215,253,65,85,208,76,62,3,237,55,89,
                     232,50,217,64,244,157,199,121,252,90,17,212,203,149,152,140,187,234,177,73,174,
                     193,100,192,143,97,53,145,135,19,103,13,90,135,151,199,91,239,247,33,39,145,
                     101,120,99,3,186,86,99,41,237,203,111,79,220,135,158,42,30,154,120,67,87,167,
                     135,176,183,191,253,115,184,21,233,58,129,233,142,39,128,211,118,137,139,255,
                     114,20,218,113,154,27,127,246,250,1,8,198,250,209,92,222,173,21,88,102,219};

int noise2(int x, int y)
{
    int tmp = hash[(y + time(NULL)) % 256];
    return hash[(tmp + x) % 256];
}

float lin_inter(float x, float y, float s)
{
    return x + s * (y-x);
}

float smooth_inter(float x, float y, float s)
{
    return lin_inter(x, y, s * s * (3-2*s));
}

float noise2d(float x, float y)
{
    int x_int = x;
    int y_int = y;
    float x_frac = x - x_int;
    float y_frac = y - y_int;
    int s = noise2(x_int, y_int);
    int t = noise2(x_int+1, y_int);
    int u = noise2(x_int, y_int+1);
    int v = noise2(x_int+1, y_int+1);
    float low = smooth_inter(s, t, x_frac);
    float high = smooth_inter(u, v, x_frac);
    return smooth_inter(low, high, y_frac);
}



static float perlinNoise(float x, float y, float freq, int depth) {
    float xa = x*freq;
    float ya = y*freq;
    float amp = 1.0;
    float fin = 0;
    float div = 0.0;

    int i;
    for(i=0; i<depth; i++)
    {
        div += 256 * amp;
        fin += noise2d(xa, ya) * amp;
        amp /= 2;
        xa *= 2;
        ya *= 2;
    }

    return fin/div;
}

static int generate_pixel_colors(Uint32 pixel_colors[MAP_HEIGHT][MAP_WIDTH]) {
    float n;
    int c;
    //int* pP_table = initPTable();
    //if (!pP_table) {
    //    return 0;
    //}

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            n = perlinNoise(x, y, 0.015, 8);
            // Restircting the value of n to be between 0 and 1 instead of -1 and 1
            n += 1.0;
            n /= 2.0;
            c = (int)(n * 255);
            pixel_colors[y][x] = encodeColor(c, c, c, 255);
        }
    }
    //free(pP_table);
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
            if (!allocObj(terrain_map, &map_i, x* TILE_SQUARE_SIDE, y* TILE_SQUARE_SIDE, 1* TILE_SQUARE_SIDE,1* TILE_SQUARE_SIDE, pixel_colors[y][x], 0)) {
                return NULL;
            }
        }
    }
    terrain_map[map_i] = NULL;
    return terrain_map;
}